$bks = [ordered]@{
    "r100_p0_m100_s1.txt" = 4077.43
    "r100_p25_m75_s2.txt" = 5198.05
    "r100_p50_m50_s3.txt" = 4451.04
    "r100_p75_m25_s4.txt" = 4505.92
    "r100_p100_m0_s5.txt" = 3647.43
}

foreach ($inst in $bks.Keys) {
    Write-Host "Running $inst (BKS: $($bks[$inst]))..."
    $best_benefit = -1000000.0
    for ($seed = 1; $seed -le 5; $seed++) {
        $out = wsl ./cmake-build-release/SoICT2026 --instance "data/synthetic/$inst" --seed $seed --time-limit 20 --out "dummy_$inst" 2>&1
        $lastLine = $out[-1]
        if ($lastLine -match "benefit=([\d\.]+)") {
            $benefit = [double]$matches[1]
            if ($benefit -gt $best_benefit) {
                $best_benefit = $benefit
            }
        }
    }
    
    if ($best_benefit -gt -1000000.0) {
        $gap = [math]::Round($best_benefit - $bks[$inst], 2)
        Write-Host "  Result: $best_benefit (Diff vs BKS: $gap)"
    } else {
        Write-Host "  Result: Error"
    }
}
