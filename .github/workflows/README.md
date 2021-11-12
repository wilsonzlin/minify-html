# GitHub Actions

This folder contains various GitHub Actions that are run upon every new version Git tag pushed to GitHub. They build, pack, and upload/deploy/publish packages and binaries for using the newly released version in various programming languages and operating systems.

## Self-hosted runners

GitHub currently doesn't provide ARM64 macOS and Linux runners, so we run self-hosted versions. Jobs run on self-hosted machines aren't isolated (e.g. files created during a job run persist on the real filesystem), and most @actions/* don't have ARM64 builds yet, so self-hosted machines should have tools preinstalled before starting the GitHub Actions runner. These include:

- Go
- Rust
- Node.js
- Python/pyenv

See each workflow YAML file for the specific programs that need to be installed beforehand.
