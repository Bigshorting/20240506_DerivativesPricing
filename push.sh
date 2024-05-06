#! /bin/bash

cd $(dirname $0)
git add -u
git status
while true; do
    read -p "Do you want to continue? (y/n): " choice
    case $choice in
        y|Y)
            echo "You chose to continue."
            time=$(date "+%Y-%m-%d %H:%M:%S")
            git commit -m"commit at ${time}: $1"
            git push origin main
            break
            ;;
        n|N)
            echo "You chose to exit."
            exit 0
            ;;
        *)
            echo "Invalid choice. Please enter 'y' or 'n'."
            ;;
    esac
done