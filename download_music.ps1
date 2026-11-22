# Helper script to download playlist audio files for Last Delivery
$musicDir = Join-Path $PSScriptRoot "assets\music"

$tracks = @(
    @{ name = "jeena_jeena.mp3"; url = "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-1.mp3" },
    @{ name = "oh_oh_jane_jaana.mp3"; url = "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-2.mp3" },
    @{ name = "o_re_piya.mp3"; url = "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-3.mp3" }
)

Write-Host "Checking game music tracks in $musicDir..." -ForegroundColor Cyan

foreach ($t in $tracks) {
    $target = Join-Path $musicDir $t.name
    if (-not (Test-Path $target)) {
        Write-Host "Downloading $($t.name)..." -ForegroundColor Yellow
        try {
            Invoke-WebRequest -Uri $t.url -OutFile $target -TimeoutSec 30
            Write-Host "Saved $($t.name)" -ForegroundColor Green
        } catch {
            Write-Warning "Could not download $($t.name). You can place any mp3 file with this name in assets/music/ manually."
        }
    } else {
        Write-Host "Found $($t.name) (ready)" -ForegroundColor Green
    }
}

Write-Host "Music setup complete! Run LastDelivery.exe to play." -ForegroundColor Cyan
