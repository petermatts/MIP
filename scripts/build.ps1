param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildType = "Debug",

    [string]$Generator = "Ninja"
)

$ErrorActionPreference = "Stop"

# Resolve the project root relative to this script.
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "Configuring mip..."

cmake -S $ProjectRoot -B $BuildDir `
    -G $Generator `
    -DMIP_BUILD_TESTS=ON `
    -DMIP_BUILD_EXAMPLES=ON

if ($LASTEXITCODE -ne 0) {
    throw "CMake configuration failed."
}

Write-Host "Building mip..."

cmake --build $BuildDir `
    --config $BuildType `
    --parallel

if ($LASTEXITCODE -ne 0) {
    throw "Build failed."
}

Write-Host "Running tests..."

ctest --test-dir $BuildDir `
    -C $BuildType `
    --output-on-failure

if ($LASTEXITCODE -ne 0) {
    throw "Tests failed."
}

Write-Host "Build and tests completed successfully."