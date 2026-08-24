#!/usr/bin/env bash
# Creates docker/mosquitto/security/passfile for broker authentication.
# The passfile is gitignored - run this once before starting the stack.
#
# Usage: ./create-passfile.sh USERNAME PASSWORD [USERNAME PASSWORD ...]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SECURITY_DIR="$SCRIPT_DIR/security"
PASSFILE="$SECURITY_DIR/passfile"

if [ $# -lt 2 ] || [ $(($# % 2)) -ne 0 ]; then
    echo "Usage: $0 USERNAME PASSWORD [USERNAME PASSWORD ...]" >&2
    exit 1
fi

mkdir -p "$SECURITY_DIR"
rm -f "$PASSFILE"

first=true
while [ $# -gt 0 ]; do
    if [ "$first" = true ]; then
        cmd="mosquitto_passwd -b -c /sec/passfile"
        first=false
    else
        cmd="mosquitto_passwd -b /sec/passfile"
    fi
    # chmod inside the container: mosquitto drops privileges and must be able
    # to read the passfile; files created here are root-owned otherwise.
    docker run --rm -v "$SECURITY_DIR":/sec eclipse-mosquitto sh -c \
        "$cmd \"\$1\" \"\$2\" && chmod 0644 /sec/passfile" _ "$1" "$2"
    shift 2
done

echo "Created $PASSFILE"
