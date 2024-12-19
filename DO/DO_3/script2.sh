#!/bin/bash

# Gather system information
HOSTNAME=$(hostname)
TIMEZONE=$(cat /etc/timezone) "UTC $(date +'%z' | sed 's/\(..\)\(..\)/-\1/')"
USER=$(whoami)
OS=$(lsb_release -d | awk -F"\t" '{print $2}')
DATE=$(date +"%d %B %Y %H:%M:%S")
UPTIME=$(uptime -p)
UPTIME_SEC=$(cat /proc/uptime | awk '{print $1}' | cut -d. -f1)
IP=$(hostname -I | awk '{print $1}')
MASK=$(ip addr show | grep "inet " | awk '{print $2}' | cut -d'/' -f2 | xargs -I {} ipcalc -m 0.0.0.0/{}/ | grep Netmask | awk '{print $2}')
GATEWAY=$(ip route | grep default | awk '{print $3}')
RAM_TOTAL=$(free -g | awk '/Mem:/ {printf "%.3f GB\n", $2}')
RAM_USED=$(free -g | awk '/Mem:/ {printf "%.3f GB\n", $3}')
RAM_FREE=$(free -g | awk '/Mem:/ {printf "%.3f GB\n", $4}')
SPACE_ROOT=$(df / | awk 'NR==2 {printf "%.2f MB\n", $2 / 1024}')
SPACE_ROOT_USED=$(df / | awk 'NR==2 {printf "%.2f MB\n", $3 / 1024}')
SPACE_ROOT_FREE=$(df / | awk 'NR==2 {printf "%.2f MB\n", $4 / 1024}')

# Output gathered information
echo "HOSTNAME = $HOSTNAME"
echo "TIMEZONE = $TIMEZONE"
echo "USER = $USER"
echo "OS = $OS"
echo "DATE = $DATE"
echo "UPTIME = $UPTIME"
echo "UPTIME_SEC = $UPTIME_SEC"
echo "IP = $IP"
echo "MASK = $MASK"
echo "GATEWAY = $GATEWAY"
echo "RAM_TOTAL = $RAM_TOTAL"
echo "RAM_USED = $RAM_USED"
echo "RAM_FREE = $RAM_FREE"
echo "SPACE_ROOT = $SPACE_ROOT"
echo "SPACE_ROOT_USED = $SPACE_ROOT_USED"
echo "SPACE_ROOT_FREE = $SPACE_ROOT_FREE"

# Ask user if they want to save the output to a file
read -p "Do you want to save this information to a file? (Y/N): " response
if [[ $response == "Y" || $response == "y" ]]; then
    TIMESTAMP=$(date +"%d_%m_%y_%H_%M_%S")
    FILENAME="${TIMESTAMP}.status"
    {
        echo "HOSTNAME = $HOSTNAME"
        echo "TIMEZONE = $TIMEZONE"
        echo "USER = $USER"
        echo "OS = $OS"
        echo "DATE = $DATE"
        echo "UPTIME = $UPTIME"
        echo "UPTIME_SEC = $UPTIME_SEC"
        echo "IP = $IP"
        echo "MASK = $MASK"
        echo "GATEWAY = $GATEWAY"
        echo "RAM_TOTAL = $RAM_TOTAL"
        echo "RAM_USED = $RAM_USED"
        echo "RAM_FREE = $RAM_FREE"
        echo "SPACE_ROOT = $SPACE_ROOT"
        echo "SPACE_ROOT_USED = $SPACE_ROOT_USED"
        echo "SPACE_ROOT_FREE = $SPACE_ROOT_FREE"
    } > "$FILENAME"
    echo "Information saved to $FILENAME"
else
    echo "Information not saved."
fi
