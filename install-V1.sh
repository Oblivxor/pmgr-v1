#!/bin/bash
# PMGR v1 Installer Script (Linux/macOS)
# Easy one-command setup

echo "=== Installing PMGR v1 ==="

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Detected Linux OS..."
    if command -v apt &> /dev/null; then
        sudo apt update
        sudo apt install -y gcc make libsodium-dev git
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y gcc make libsodium-devel git
    else
        echo "Unsupported Linux package manager. Please install gcc, make, and libsodium manually."
        exit 1
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Detected macOS..."
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Installing..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    brew install gcc libsodium
else
    echo "Unsupported OS. Manual install required."
    exit 1
fi

# Compile PMGR v1
echo "Compiling PMGR v1..."
gcc pmgr-V1.0.c -o pmgr -lsodium

# Optional: move binary to /usr/local/bin for easy access
sudo mv pmgr /usr/local/bin/pmgr

echo "Installation complete! You can now run:"
echo "  pmgr"
