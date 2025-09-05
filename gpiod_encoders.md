## Me
I'm going to have a number of rotary encoders connected to my RPi. How might I practically monitor these using libgpiod?

## AI Assistant
Great question. Rotary encoders are a perfect fit for libgpiod’s edge-event API, and you can scale to many encoders if you structure it right. Here’s a practical approach that works well on Raspberry Pi with libgpiod v2.x.

High-level plan
- Hardware
  - Use the same ground and 3V3 for all encoders.
  - Prefer hardware pull-ups (e.g., 10 kΩ to 3V3). If not, configure internal bias pull-ups via libgpiod.
  - If you can, add a small RC filter (e.g., 1–4.7 nF) for deglitching. If not, do software debounce.
- GPIO configuration
  - For each encoder, request both Phase A and Phase B as inputs with edge detection on both edges.
  - Enable bias pull-up and optionally active-low depending on wiring.
  - If your kernel/libgpiod support line-level debounce (a.k.a. glitch filter), enable it; otherwise implement software debounce.
- Event model
  - Create one request per GPIO chip for all lines you need on that chip (better performance). Lines in a single request must belong to the same chip.
  - Use the request’s pollable file descriptor with poll/ppoll/epoll in a single event loop thread.
  - Read edge events in batches and dispatch to a per-encoder state machine.
- Decoding
  - Maintain the last 2-bit AB state per encoder.
  - Use a state table to map transitions to +1/-1 steps; ignore invalid transitions (caused by bounce).
  - Optionally apply a minimum inter-event time to filter bounce (e.g., 500–3000 µs depending on encoder).
- Concurrency and performance
  - One event thread is usually enough for many encoders; decoding is very light.
  - If you do heavy processing per-step, enqueue results to a lock-free queue for a worker.
  - Consider raising thread priority a bit (SCHED_FIFO/rr with care) if you need very low latency.

Data structures you’ll want
- A structure describing each encoder:
  - the two line offsets (A and B)
  - current 2-bit state
  - last event timestamp for debounce
  - an accumulator/counter
- A reverse index mapping from line offset → encoder ID and phase (A or B) so you can route events quickly.

Example flow in C++ (libgpiod v2 style)
- Request lines on a chip:
  - One request for multiple lines is preferred (all A/B lines for encoders on the same GPIO chip).
  - Configure input, bias, both edges, optional debounce if supported.
- Event loop:
  - poll on the request fd
  - when readable, read edge events into a buffer
  - for each event:
    - identify which encoder and which phase changed
    - read current levels of A and B from the event or re-sample both lines (depending on your approach; events contain the line that changed and the edge type, but you may also want the other phase’s current level)
    - run the quadrature state machine to increment/decrement the counter

Minimal quadrature decode logic
- Legal sequence for clockwise (Gray code): 00 → 01 → 11 → 10 → 00
- Counter-clockwise is the reverse.
- Use a 16-entry lookup table keyed by (prev_state << 2) | curr_state to produce −1/0/+1; anything else is noise.

Illustrative code sketch (error checks omitted for brevity; adjust names to your codebase):

```c++
#include <gpiod.h>
#include <poll.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

struct Encoder {
    uint32_t a_offset;
    uint32_t b_offset;
    uint8_t  state;        // 2-bit: (A<<1)|B
    int32_t  count;
    uint64_t last_ns;      // for debounce
};

static int8_t quad_table[16] = {
/* prev:00 */  0, +1,  0, -1,
/* prev:01 */ -1,  0, +1,  0,
/* prev:10 */  0, -1,  0, +1,
/* prev:11 */ +1,  0, -1,  0
};

int main() {
    // 1) Open the chip (e.g., "gpiochip0")
    gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");

    // 2) Define all line offsets you need (A and B for each encoder on this chip)
    std::vector<uint32_t> offsets = { 5,6, 13,19, 20,21 }; // example: 3 encoders

    // 3) Configure lines
    gpiod_line_config* lcfg = gpiod_line_config_new();
    gpiod_line_settings* ls = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(ls, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(ls, GPIOD_LINE_BIAS_PULL_UP);
    gpiod_line_settings_set_edge_detection(ls, GPIOD_LINE_EDGE_BOTH);
    // If supported by your kernel/libgpiod:
    // gpiod_line_settings_set_debounce(ls, 1000'000); // 1 ms in ns, if available
    gpiod_line_config_add_line_settings(lcfg, offsets.data(), offsets.size(), ls);
    gpiod_line_settings_free(ls);

    gpiod_request_config* rcfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(rcfg, "rotary-monitor");

    gpiod_request* req = gpiod_chip_request_lines(chip, rcfg, lcfg);
    gpiod_request_config_free(rcfg);
    gpiod_line_config_free(lcfg);

    // 4) Build maps: offset -> (encoder id, phase)
    struct Phase { int enc_id; bool isA; };
    std::unordered_map<uint32_t, Phase> offset_to_phase;

    std::vector<Encoder> encs;
    // Example assignment
    for (int i = 0; i < (int)offsets.size(); i += 2) {
        Encoder e{};
        e.a_offset = offsets[i];
        e.b_offset = offsets[i+1];
        e.count = 0;
        e.state = 0; // will initialize after reading current levels
        encs.push_back(e);
        offset_to_phase[e.a_offset] = { (int)encs.size()-1, true };
        offset_to_phase[e.b_offset] = { (int)encs.size()-1, false };
    }

    // 5) Initialize current AB state per encoder by reading values
    for (auto& e : encs) {
        int a = gpiod_request_get_value(req, e.a_offset);
        int b = gpiod_request_get_value(req, e.b_offset);
        e.state = ((a & 1) << 1) | (b & 1);
    }

    // 6) Event loop
    int fd = gpiod_request_get_fd(req); // pollable fd
    gpiod_edge_event_buffer* buf = gpiod_edge_event_buffer_new(64);

    struct pollfd pfd{ fd, POLLIN, 0 };
    while (true) {
        int pr = poll(&pfd, 1, /*timeout ms*/ 500);
        if (pr <= 0) continue; // timeout or interrupted; loop

        while (true) {
            int n = gpiod_request_read_edge_events(req, buf, 64);
            if (n <= 0) break; // no more events (0) or EAGAIN

            for (int i = 0; i < n; ++i) {
                const gpiod_edge_event* ev = gpiod_edge_event_buffer_get_event(buf, i);
                uint32_t off = gpiod_edge_event_get_line_offset(ev);
                auto it = offset_to_phase.find(off);
                if (it == offset_to_phase.end()) continue;

                const auto [enc_id, isA] = it->second;
                auto& e = encs[enc_id];

                // Optional software debounce using timestamps
                uint64_t t_ns = gpiod_edge_event_get_timestamp_ns(ev);
                if (t_ns - e.last_ns < 500'000) { // 0.5 ms
                    continue;
                }
                e.last_ns = t_ns;

                // Read both levels to determine new state (robust against bounce)
                int a = gpiod_request_get_value(req, e.a_offset);
                int b = gpiod_request_get_value(req, e.b_offset);
                uint8_t new_state = ((a & 1) << 1) | (b & 1);

                uint8_t idx = ((e.state & 0x3) << 2) | (new_state & 0x3);
                int8_t step = quad_table[idx];
                if (step != 0) {
                    e.count += step;
                    e.state = new_state;
                    // Notify or queue the update as needed:
                    // on_step(enc_id, step, e.count);
                } else {
                    // If desired, still update state when stable:
                    e.state = new_state;
                }
            }
        }
    }

    gpiod_edge_event_buffer_free(buf);
    gpiod_line_request_release(req);
    gpiod_chip_close(chip);
    return 0;
}
```


Notes and gotchas
- Group lines by chip: Raspberry Pi may expose multiple gpiochip devices; request lines that belong to the same chip in one request. Create multiple requests if encoders span chips, and add all their fds to poll/epoll.
- Bias and active low: If you wire encoders to ground with pull-ups, edges are falling on detent. You can leave active-high and just handle both edges, or set active-low if you prefer.
- Debounce:
  - Hardware is best.
  - Kernel-level debounce/glitch filter works if available (depends on SoC and kernel).
  - Software debounce based on timestamps is usually sufficient for mechanical encoders; 0.5–3 ms typical.
- Throughput: Even 10 encoders spun quickly generate only a few k events/s—well within what a single thread can manage.
- Permissions: Ensure your user can access /dev/gpiochipN (udev rules or run as root).
- Alternative: The Linux kernel has a rotary-encoder driver that can expose input events, which is even simpler if your encoders are fixed and described via device tree. Since you asked for libgpiod, the above keeps everything in userspace.

Scaling to “a number” of encoders
- Maintain one event loop per chip with epoll for multiple requests.
- Use a fixed-size event buffer (e.g., 64 or 256) and drain until empty each time fd is readable.
- Keep per-encoder counters and expose them via your app’s API; optionally coalesce updates to reduce chatter.

This pattern is robust, low-latency, and easy to extend as you add more encoders. If you want, I can adapt this into a small class that takes a list of encoders and provides callbacks for step events.

