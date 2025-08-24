sudo apt install -y gpiod libgpiod-dev

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
