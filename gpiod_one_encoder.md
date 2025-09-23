## Me
How might I use libgpiod on an RPi to read a rotary encoder?

## AI Assistant
Here’s a compact, practical way to read a 2‑channel quadrature (A/B) rotary encoder on a Raspberry Pi using libgpiod. It covers wiring, event handling, decoding, debounce, and examples for both libgpiod v1 and v2 APIs.

What you need to know first
- Wiring
  - Connect encoder A to a GPIO line, encoder B to another GPIO line.
  - Connect encoder common (C/COM) to GND.
  - Use internal pull-ups (preferred) or external 10 kΩ pull-ups to 3.3 V.
- Permissions
  - Add your user to the gpio group and re-login:
    - sudo usermod -aG gpio $USER
- Discover lines
  - gpiodetect and gpioinfo help identify the chip name (e.g., gpiochip0) and line offsets for your chosen pins.

Quadrature decoding basics
- Read both A and B levels each time either changes, and use a 2-bit state transition table:
  - prev = (prevA << 1) | prevB
  - curr = (currA << 1) | currB
  - idx = (prev << 2) | curr
  - dir[idx] gives −1, 0, or +1
- A common 4-step table:
  - static const int8_t dir[16] = {0,-1,+1,0, +1,0,0,-1, -1,0,0,+1, 0,+1,-1,0};
- Many encoders generate 4 transitions per detent; divide the accumulated count by 4 if you want “detents.”

Debounce
- Mechanical encoders bounce; use:
  - Time-based filtering: ignore transitions that occur within, say, 500–1500 microseconds of the last accepted edge on the same line.
  - Or only accept direction when a full valid sequence completes (the 4-step method above is fairly resilient).
- libgpiod provides timestamps on events; use them to implement debounce without busy-waiting.

Example: libgpiod v2 (current on Raspberry Pi OS Bookworm)
- Header: #include <gpiod.h>
- Key ideas:
  - Open the chip.
  - Request both lines with both-edge events, bias pull-up, active-high.
  - Poll for events, read current A/B levels on each event, run the state-machine.

```c++
// C (libgpiod v2)
#include <gpiod.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

static const int8_t dir_table[16] = {
    0,-1,+1,0, +1,0,0,-1, -1,0,0,+1, 0,+1,-1,0
};

static inline int64_t mono_us()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}

int main()
{
    const char *chip_path = "/dev/gpiochip0";
    const unsigned int lines[2] = {23, 24}; // example: GPIO 23=A, 24=B

    struct gpiod_chip *chip = gpiod_chip_open(chip_path);
    if (!chip) {
        perror("gpiod_chip_open");
        return 1;
    }

    struct gpiod_line_config *lcfg = gpiod_line_config_new();
    if (!lcfg) return 1;

    // Configure both lines: input, bias pull-up, both-edge events
    struct gpiod_line_settings *ls = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(ls, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(ls, GPIOD_LINE_BIAS_PULL_UP);
    gpiod_line_settings_set_edge_detection(ls, GPIOD_LINE_EDGE_BOTH);
    gpiod_line_config_add_line_settings(lcfg, lines, 2, ls);
    gpiod_line_settings_free(ls);

    struct gpiod_request_config *rcfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(rcfg, "rotary-encoder");

    struct gpiod_line_request *req =
        gpiod_chip_request_lines(chip, rcfg, lcfg);
    gpiod_request_config_free(rcfg);
    gpiod_line_config_free(lcfg);

    if (!req) {
        perror("gpiod_chip_request_lines");
        gpiod_chip_close(chip);
        return 1;
    }

    // Read initial state
    int vals[2];
    if (gpiod_line_request_get_values(req, vals) < 0) {
        perror("get_values");
        return 1;
    }
    int prevA = vals[0] ? 1 : 0;
    int prevB = vals[1] ? 1 : 0;

    int32_t count = 0;
    int64_t last_event_us[2] = {0, 0};
    const int debounce_us = 1000; // adjust to taste (500–2000 typical)

    while (1) {
        // Wait up to 1 second for any edge on either line
        int ret = gpiod_line_request_wait_edge_events(req, 1000000000ULL);
        if (ret < 0) { perror("wait_edge"); break; }
        if (ret == 0) continue; // timeout

        // Read and drain all pending events
        while (1) {
            struct gpiod_edge_event_buffer *buf = gpiod_edge_event_buffer_new(8);
            int n = gpiod_line_request_read_edge_events(req, buf, 8);
            if (n <= 0) {
                gpiod_edge_event_buffer_free(buf);
                break;
            }

            for (int i = 0; i < n; ++i) {
                const struct gpiod_edge_event *ev = gpiod_edge_event_buffer_get_event(buf, i);
                unsigned int off = gpiod_edge_event_get_line_offset(ev);
                uint64_t ts = gpiod_edge_event_get_timestamp_ns(ev) / 1000ULL; // us
                int idx = (off == lines[0]) ? 0 : (off == lines[1]) ? 1 : -1;
                if (idx < 0) continue;

                // Simple debounce
                if (ts - last_event_us[idx] < debounce_us) continue;
                last_event_us[idx] = ts;

                // Read instantaneous A/B and decode
                if (gpiod_line_request_get_values(req, vals) < 0) continue;
                int a = vals[0] ? 1 : 0;
                int b = vals[1] ? 1 : 0;

                int prev = (prevA << 1) | prevB;
                int curr = (a << 1) | b;
                int step = dir_table[(prev << 2) | curr];

                if (step != 0) {
                    count += step;
                    // Optional: if you want detents rather than 4-step:
                    // if (count % 4 == 0) printf("detent: %d\n", count / 4);
                    printf("count: %d\n", count);
                }

                prevA = a;
                prevB = b;
            }

            gpiod_edge_event_buffer_free(buf);
        }
    }

    gpiod_line_request_release(req);
    gpiod_chip_close(chip);
    return 0;
}
```


Example: libgpiod v1 (older API; still common in tutorials)
- Header: #include <gpiod.h>
- Key calls:
  - gpiod_chip_open_by_name / gpiod_chip_open
  - gpiod_chip_get_line
  - gpiod_line_request_both_edges_events_flags
  - gpiod_line_event_wait / gpiod_line_event_read
  - gpiod_line_get_value

```c++
// C (libgpiod v1)
#include <gpiod.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

static const int8_t dir_table[16] = {
    0,-1,+1,0, +1,0,0,-1, -1,0,0,+1, 0,+1,-1,0
};

static inline int64_t mono_us()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}

int main()
{
    const char *chip_name = "gpiochip0";
    int a_line = 23, b_line = 24;

    struct gpiod_chip *chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) { perror("chip_open"); return 1; }

    struct gpiod_line *a = gpiod_chip_get_line(chip, a_line);
    struct gpiod_line *b = gpiod_chip_get_line(chip, b_line);
    if (!a || !b) { perror("get_line"); return 1; }

    if (gpiod_line_request_both_edges_events_flags(
            a, "enc", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
        perror("req a"); return 1;
    }
    if (gpiod_line_request_both_edges_events_flags(
            b, "enc", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0) {
        perror("req b"); return 1;
    }

    int prevA = gpiod_line_get_value(a) > 0;
    int prevB = gpiod_line_get_value(b) > 0;
    int32_t count = 0;
    int64_t lastA = 0, lastB = 0;
    const int debounce_us = 1000;

    while (1) {
        struct timespec timeout = { .tv_sec = 1, .tv_nsec = 0 };
        int ra = gpiod_line_event_wait(a, &timeout);
        int rb = gpiod_line_event_wait(b, &timeout);
        if (ra < 0 || rb < 0) { perror("event_wait"); break; }

        // Drain any events on A and B
        struct gpiod_line_event ev;
        while (gpiod_line_event_read(a, &ev) == 0) {
            int64_t now = mono_us();
            if (now - lastA < debounce_us) continue;
            lastA = now;

            int va = gpiod_line_get_value(a) > 0;
            int vb = gpiod_line_get_value(b) > 0;

            int prev = (prevA << 1) | prevB;
            int curr = (va << 1) | vb;
            int step = dir_table[(prev << 2) | curr];
            if (step) { count += step; printf("count: %d\n", count); }

            prevA = va; prevB = vb;
        }
        while (gpiod_line_event_read(b, &ev) == 0) {
            int64_t now = mono_us();
            if (now - lastB < debounce_us) continue;
            lastB = now;

            int va = gpiod_line_get_value(a) > 0;
            int vb = gpiod_line_get_value(b) > 0;

            int prev = (prevA << 1) | prevB;
            int curr = (va << 1) | vb;
            int step = dir_table[(prev << 2) | curr];
            if (step) { count += step; printf("count: %d\n", count); }

            prevA = va; prevB = vb;
        }
    }

    gpiod_line_release(a);
    gpiod_line_release(b);
    gpiod_chip_close(chip);
    return 0;
}
```


Build tips
- If using pkg-config:
  - cc enc.c -o enc $(pkg-config --cflags --libs libgpiod)
- If you see undefined refs, check which libgpiod version you have:
  - gpiod --version
  - pkg-config --modversion libgpiod

Tuning and reliability
- Choose sane debounce (start around 1 ms).
- If the encoder is noisy, consider:
  - Hardware RC filter (e.g., 1–10 nF with 10 kΩ).
  - Only count full valid 4-step cycles to avoid partial bounces.
- For very fast rotation, avoid printing in the hot path and keep the loop lightweight.
- If you need acceleration or a UI, decouple the event thread from processing via a queue.

Alternative
- The Linux kernel has a rotary-encoder driver that exposes events via the input subsystem (evdev). libgpiod is great for learning and custom logic; the kernel driver is robust if you prefer a ready-made solution.

## Me
Do any of these gpiod functions throw exceptions?

## AI Assistant
Short answer: the C libgpiod API does not throw exceptions. Some C++ bindings do.

Details:
- C API (<gpiod.h>, libgpiod v1 and v2)
  - It’s a plain C library; functions return error codes or NULL and set errno on failure.
  - Examples:
    - gpiod_chip_open(...) returns nullptr on error.
    - gpiod_chip_request_lines(...), gpiod_line_request_wait_edge_events(...), etc., return -1 on error.
  - No exceptions are thrown by these functions.

- C++ bindings (<gpiod.hpp>, “gpiodcxx”)
  - If you use the C++ wrappers, operations commonly throw exceptions (typically std::system_error) on failure.
  - This behavior differs from the C API and requires try/catch in your C++ code.

What to do in your case
- Since you’re including <gpiod.h>, you’re using the C API. Check return values and errno; nothing will throw unless you throw yourself.

## Me
Do they set an error variable (obtained by something like getLastError())?

## AI Assistant
No. The C libgpiod API does not provide a library-specific “getLastError()”. It follows standard C conventions:

- On failure, functions return NULL (for pointers) or -1 (for ints) and set errno.
- On success, they return a valid pointer or 0/positive value as documented.
- You retrieve the error via errno and format it with strerror or perror.

Typical patterns
```c++
// C
#include <gpiod.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main() {
    struct gpiod_chip *chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        fprintf(stderr, "gpiod_chip_open failed: %s\n", strerror(errno));
        return 1;
    }

    struct gpiod_line_config *lcfg = gpiod_line_config_new();
    if (!lcfg) {
        fprintf(stderr, "gpiod_line_config_new failed: %s\n", strerror(errno));
        gpiod_chip_close(chip);
        return 1;
    }

    // ... more calls that return -1 on error, check and use errno ...
    gpiod_line_config_free(lcfg);
    gpiod_chip_close(chip);
    return 0;
}
```


Notes
- errno is thread-local, so this is safe in multithreaded programs as long as you read it immediately after the failing call.
- Some calls also use special non-negative values to indicate conditions like timeouts (e.g., 0 for “no event”), which are not errors and do not set errno. Always check the specific function’s return contract.

