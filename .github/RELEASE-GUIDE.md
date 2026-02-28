# GitHub Release Guide

## Setup

To enable automated releases:

1. **Activate the workflow:**
   ```bash
   cd .github/workflows
   mv release.yml.example release.yml
   git add release.yml
   git commit -m "Enable automated releases"
   git push
   ```

2. **Ensure GitHub Actions is enabled** in your repository settings

## Creating a Release

### Method 1: Using Git Tags (Recommended)

```bash
# Make sure all changes are committed
git add .
git commit -m "Prepare release v1.0.0"
git push

# Create and push a version tag
git tag v1.0.0
git push origin v1.0.0
```

The GitHub Action will automatically:
- Build the project on **Ubuntu**, **macOS**, and **Windows**
- Run all tests on each platform
- Package executables, data files, and sample outputs
- Create a GitHub release with downloadable archives:
  - `cs471-project-Linux.tar.gz`
  - `cs471-project-macOS.tar.gz`
  - `cs471-project-Windows.zip`

### Method 2: Manual Release

1. Go to your GitHub repository
2. Click "Releases" → "Draft a new release"
3. Click "Choose a tag" → Create new tag (e.g., `v1.0.0`)
4. Fill in release title and description
5. Upload artifacts manually

## Version Numbering

Use [Semantic Versioning](https://semver.org/):
- `v1.0.0` - Major release (initial submission)
- `v1.1.0` - New features (additional algorithms, improvements)
- `v1.0.1` - Bug fixes

## What Gets Released

Each platform-specific archive includes:
- ✅ `cpu_sched` / `cpu_sched.exe` - CPU scheduler executable
- ✅ `cpu_sched_tests` / `cpu_sched_tests.exe` - Test suite
- ✅ `producer_consumer` / `producer_consumer.exe` - Producer-Consumer executable
- ✅ `producer_consumer_tests` / `producer_consumer_tests.exe` - Test suite
- ✅ Sample input data files
- ✅ Sample output files (when available)
- ✅ README documentation

## Testing Before Release

Always test locally before creating a release:

```bash
# Clean build
make clean
make

# Run all tests
make test

# Verify executables work
./CPUSCHED/cpu_sched
./PRODUCER-CONSUMER/producer_consumer
```

## Viewing Release Artifacts

After creating a release:
1. Go to your GitHub repository
2. Click "Releases"
3. Each release will show:
   - Release notes
   - Source code (.zip and .tar.gz)
   - Platform-specific binaries (Linux, macOS, Windows)

## Example Release Workflow

```bash
# 1. Complete implementation
git add .
git commit -m "Complete FIFO and SJF schedulers"
git push

# 2. Generate sample outputs
./CPUSCHED/cpu_sched
# This creates output files

# 3. Commit sample outputs
git add CPUSCHED/output/
git commit -m "Add sample outputs for submission"
git push

# 4. Create release
git tag v1.0.0
git push origin v1.0.0

# 5. Wait for GitHub Actions to complete (check Actions tab)
# 6. Verify release at github.com/<your-username>/course-project/releases
```

## Troubleshooting

### Workflow not triggering
- Ensure the file is named `release.yml` (not `.example`)
- Check that GitHub Actions is enabled in repository settings
- Verify the tag was pushed: `git push origin --tags`

### Build fails on Windows
- Check that all file paths use forward slashes or platform-agnostic methods
- Verify executables have `.exe` extension on Windows
- Review the Actions tab on GitHub for detailed error logs

### Build fails on Linux/macOS
- Ensure all dependencies are installed in the workflow
- Check that executable permissions are set correctly
- Review CMake configuration for platform-specific issues

### Release missing files
- Update the "Package artifacts" steps in `release.yml`
- Ensure files exist in the expected locations before release
- Check that output directory is created and populated

### Multiple releases for same tag
The workflow creates one release per tag, but uploads artifacts from all three platforms (Linux, macOS, Windows) to the same release. This is correct behavior.
