# We need version 2.2.x If the distro provides that then...
# (Important: Make sure any existing version is not v1.6.3
# If it is then remove it)
sudo apt install -y gpiod libgpiod-dev
# ...otherwise
git clone https://github.com/brgl/libgpiod.git
cd libgpiod
git fetch
git checkout v2.2.x
sudo apt install autoconf-archive
./autogen.sh
#If PC then..
./configure --prefix=/usr --libdir=/usr/lib/x86_64-linux-gnu
#If RPi then
./configure --prefix=/usr --libdir=/usr/lib/aarch64-linux-gnu
make
sudo make install


# Create a gpio group
sudo groupadd gpio || true

# Create a udev rule to give the gpio group access
sudo tee /etc/udev/rules.d/60-gpio.rules >/dev/null <<'EOF'
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", MODE="0660", GROUP="gpio", TAG+="uaccess"
EOF

# Reload rules and apply
sudo udevadm control --reload-rules
sudo udevadm trigger -c add /dev/gpiochip*

# Add your user to the group
sudo usermod -aG gpio $USER
# Log out and back in (or reboot) to pick up the new group
