Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$Screen = [System.Windows.Forms.Screen]::PrimaryScreen
$Width  = $Screen.Bounds.Width
$Height = $Screen.Bounds.Height
$Left   = $Screen.Bounds.Left
$Top    = $Screen.Bounds.Top

$Bitmap = New-Object System.Drawing.Bitmap -ArgumentList $Width, $Height
$Graphics = [System.Drawing.Graphics]::FromImage($Bitmap)
$Graphics.CopyFromScreen($Left, $Top, 0, 0, $Bitmap.Size)

$OutputPath = "C:\Users\alyss\.gemini\antigravity\brain\09a26fce-03df-41e6-b76f-88ecf05d5035\debug_screenshot.png"
$Bitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)

$Graphics.Dispose()
$Bitmap.Dispose()
Write-Host "Screenshot saved to $OutputPath"
