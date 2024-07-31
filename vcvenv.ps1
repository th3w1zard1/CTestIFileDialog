param (
    [string]$command,
    [string]$subcommand,
    [string]$packageName,
    [string]$requirementsFile,
    [switch]$help
)

function Show_Help {
    Write-Host @"
vcvenv.ps1 - A PowerShell script to manage C++ virtual environments and packages using vcpkg

Usage:
    .\vcvenv.ps1 <command> [<subcommand>] [<options>]

Commands:
    help                           Show this help message.
    new <venvName>                 Create a new C++ virtual environment.
    activate <venvName>            Activate an existing C++ virtual environment.
    deactivate                     Deactivate the current C++ virtual environment.
    install <packageName>          Install a package in the activated environment.
    remove <packageName>           Remove a package from the activated environment.
    list                           List all installed packages in the activated environment.
    search <packageName>           Search for a package in vcpkg repositories.
    freeze                         Freeze installed packages to a requirements.txt file.
    install-requirements <file>    Install packages from a requirements.txt file.
    upgrade                        Upgrade all installed packages to their latest versions.
    upgrade <packageName>          Upgrade a specific package to the latest version.
    uninstall-env <venvName>       Uninstall a virtual environment.
    List_All_VENVcs                      List all created virtual environments.
    Env_Info                       Show information about the current environment.
    dependencies                   Show dependency tree of installed packages.
    rename-env <oldName> <newName> Rename a virtual environment.
    clone-env <srcName> <dstName>  Clone an existing virtual environment.
    update-script                  Update this script to the latest version.

Use '.\vcvenv.ps1 <command> --help' for detailed information on each command.
"@
}

function Show_Command_Help {
    param ([string]$command)
    $helpMessages = @{
        "new"                   = "Usage:\n    .\vcvenv.ps1 new <venvName>\n\nCreates a new C++ virtual environment with the specified name."
        "activate"              = "Usage:\n    .\vcvenv.ps1 activate <venvName>\n\nActivates an existing C++ virtual environment."
        "deactivate"            = "Usage:\n    .\vcvenv.ps1 deactivate\n\nDeactivates the current C++ virtual environment."
        "install"               = "Usage:\n    .\vcvenv.ps1 install <packageName>\n\nInstalls a package using vcpkg in the activated environment."
        "remove"                = "Usage:\n    .\vcvenv.ps1 remove <packageName>\n\nRemoves a package using vcpkg from the activated environment."
        "list"                  = "Usage:\n    .\vcvenv.ps1 list\n\nLists all installed packages in the activated environment."
        "search"                = "Usage:\n    .\vcvenv.ps1 search <packageName>\n\nSearches for a package in the vcpkg repository."
        "freeze"                = "Usage:\n    .\vcvenv.ps1 freeze\n\nFreezes the current list of installed packages into a requirements.txt file."
        "install-requirements"  = "Usage:\n    .\vcvenv.ps1 install-requirements <file>\n\nInstalls packages from a requirements.txt file."
        "upgrade"               = "Usage:\n    .\vcvenv.ps1 upgrade [<packageName>]\n\nUpgrades all or a specific package to the latest version."
        "uninstall-env"         = "Usage:\n    .\vcvenv.ps1 uninstall-env <venvName>\n\nUninstalls the specified virtual environment."
        "List_All_VENVcs"             = "Usage:\n    .\vcvenv.ps1 List_All_VENVcs\n\nLists all created virtual environments."
        "Env_Info"              = "Usage:\n    .\vcvenv.ps1 Env_Info\n\nShows detailed information about the current environment."
        "dependencies"          = "Usage:\n    .\vcvenv.ps1 dependencies\n\nShows the dependency tree of installed packages."
        "rename-env"            = "Usage:\n    .\vcvenv.ps1 rename-env <oldName> <newName>\n\nRenames a virtual environment."
        "clone-env"             = "Usage:\n    .\vcvenv.ps1 clone-env <srcName> <dstName>\n\nClones an existing virtual environment."
        "update-script"         = "Usage:\n    .\vcvenv.ps1 update-script\n\nUpdates this script to the latest version."
    }

    if ($helpMessages.ContainsKey($command)) { Write-Host $helpMessages[$command] }
    else {
        Write-Error "Unknown command: $command"
        Show_Help
    }
}

function Handle_Error([Parameter(Mandatory=$true)] [System.Management.Automation.ErrorRecord]$ErrorRecord) {
    Write-Host -ForegroundColor Red "Error Report:`nMessage: $($ErrorRecord.Exception.Message)"
    if ($ErrorRecord.InvocationInfo -and $ErrorRecord.InvocationInfo.MyCommand) {
        Write-Host -ForegroundColor Red "Command Name: $($ErrorRecord.InvocationInfo.MyCommand.Name)`nScript Name: $($ErrorRecord.InvocationInfo.ScriptName)`nLine Number: $($ErrorRecord.InvocationInfo.ScriptLineNumber)`nLine: $($ErrorRecord.InvocationInfo.Line)"
    } else { Write-Host -ForegroundColor Red "No invocation information available." }

    if ($ErrorRecord.ScriptStackTrace) {
        Write-Host -ForegroundColor Red "Script Stack Trace:`n$($ErrorRecord.ScriptStackTrace)"
    } else { Write-Host -ForegroundColor Red "No script stack trace available." }
}

function Safe_Path_Exists([string]$path) { try {return (Test-Path $path )} catch { return $false } }

function Show_Error_And_Exit {
    param ([string]$message)
    Write-Error $message
    exit 1
}

if ($help -or -not $command) {
    Show_Help
    exit 0
}

$activateScriptContent = @"
Param(
    [Parameter(Mandatory = `$false)]
    [String]
    `$VenvDir,
    [Parameter(Mandatory = `$false)]
    [String]
    `$Prompt
)

function global:deactvcpkg ([switch]`$NonDestructive) {
    # Revert to original values

    # The prior prompt:
    if (Test-Path -Path Function:_OLD_CPP_PROMPT) {
        Copy-Item -Path Function:_OLD_CPP_PROMPT -Destination Function:prompt
        Remove-Item -Path Function:_OLD_CPP_PROMPT
    }

    # The prior VCPKG_ROOT:
    if (Test-Path -Path Env:_OLD_VCPKG_ROOT) {
        Copy-Item -Path Env:_OLD_VCPKG_ROOT -Destination Env:VCPKG_ROOT
        Remove-Item -Path Env:_OLD_VCPKG_ROOT
    }

    # The prior PATH:
    if (Test-Path -Path Env:_OLD_CPP_PATH) {
        Copy-Item -Path Env:_OLD_CPP_PATH -Destination Env:PATH
        Remove-Item -Path Env:_OLD_CPP_PATH
    }

    # Just remove the CPP_VENV altogether:
    if (Test-Path -Path Env:CPP_VENV) {
        Remove-Item -Path env:CPP_VENV
    }

    # Just remove CPP_VENV_PROMPT altogether.
    if (Test-Path -Path Env:CPP_VENV_PROMPT) {
        Remove-Item -Path env:CPP_VENV_PROMPT
    }

    # Just remove the _CPP_VENV_PROMPT_PREFIX altogether:
    if (Get-Variable -Name "_CPP_VENV_PROMPT_PREFIX" -ErrorAction SilentlyContinue) {
        Remove-Variable -Name _CPP_VENV_PROMPT_PREFIX -Scope Global -Force
    }

    # Remove overridden vcpkg functions
    if (Test-Path -Path Function:vcpkg) {
        Remove-Item -Path Function:vcpkg
    }

    if (Test-Path -Path Function:vcpkg.exe) {
        Remove-Item -Path Function:vcpkg.exe
    }

    # Leave deactivate function in the global namespace if requested:
    if (-not `$NonDestructive) {
        Remove-Item -Path function:deactvcpkg
    }
}

<# Begin Activate script --------------------------------------------------- #>

# Determine the containing directory of this script
`$VenvExecPath = Split-Path -Parent `$MyInvocation.MyCommand.Definition
`$VenvExecDir = Get-Item -Path `$VenvExecPath

Write-Verbose "Activation script is located in path: '`$VenvExecPath'"
Write-Verbose "VenvExecDir Fullname: '`$(`$VenvExecDir.FullName)'"
Write-Verbose "VenvExecDir Name: '`$(`$VenvExecDir.Name)'"

# Set values required in priority: CmdLine, ConfigFile, Default
# First, get the location of the virtual environment, it might not be
# VenvExecDir if specified on the command line.
if (`$VenvDir) {
    Write-Verbose "VenvDir given as parameter, using '`$VenvDir' to determine values"
}
else {
    Write-Verbose "VenvDir not given as a parameter, using parent directory name as VenvDir."
    `$VenvDir = `$VenvExecDir.Parent.FullName.TrimEnd("\\/")
    Write-Verbose "VenvDir=`$VenvDir"
}

# Next, set the prompt from the command line, or just use the name of the virtual environment folder.
if (`$Prompt) {
    Write-Verbose "Prompt specified as argument, using '`$Prompt'"
}
else {
    Write-Verbose "Prompt not specified as argument to script, using directory name as prompt"
    `$Prompt = Split-Path -Path `$VenvDir -Leaf
}

Write-Verbose "Prompt = '`$Prompt'"
Write-Verbose "VenvDir='`$VenvDir'"

# Deactivate any currently active virtual environment, but leave the
# deactivate function in place.
deactvcpkg -NonDestructive

# Now set the environment variable CPP_VENV, used by many tools to determine
# that there is an activated venv.
`$env:CPP_VENV = `$VenvDir
`$env:CPP_VENV_PROMPT = `$Prompt

if (-not `$Env:CPP_VENV_DISABLE_PROMPT) {

    Write-Verbose "Setting prompt to '`$Prompt'"

    # Set the prompt to include the env name
    # Make sure _OLD_CPP_PROMPT is global
    function global:_OLD_CPP_PROMPT { "" }
    Copy-Item -Path function:prompt -Destination function:_OLD_CPP_PROMPT
    New-Variable -Name _CPP_VENV_PROMPT_PREFIX -Description "C++ virtual environment prompt prefix" -Scope Global -Option ReadOnly -Visibility Public -Value `$Prompt

    function global:prompt {
        Write-Host -NoNewline -ForegroundColor Blue "(`$_CPP_VENV_PROMPT_PREFIX) "
        _OLD_CPP_PROMPT
    }
}

# Set VCPKG_ROOT to the virtual environment directory
`$env:VCPKG_ROOT = `$VenvDir

# Add the venv to the PATH
Copy-Item -Path Env:PATH -Destination Env:_OLD_CPP_PATH
`$Env:PATH = "`$VenvExecDir`$([System.IO.Path]::PathSeparator)`$Env:PATH"

# Attempt to find the original vcpkg executable path
try {
    `$vcpkgCmd = Get-Command -Name 'vcpkg' -ErrorAction Stop
    `$global:vcpkgPath = `$vcpkgCmd.Source
    Write-Host "Found vcpkg path at `$global:vcpkgPath"
} catch {
    throw "vcpkg executable not found in PATH."
}

function global:vcpkg {
    param (
        [Parameter(ValueFromRemainingArguments = `$true)]
        [string[]]`$RemainingArgs
    )

    # Append the --x-install-root argument
    `$installRootArg = "--x-install-root=`$env:CPP_VENV"
    `$commandLine = "`$global:vcpkgPath `$installRootArg `$(`$RemainingArgs -join ' ')"
    Write-Host "Executing: `$commandLine"
    & `$global:vcpkgPath `$installRootArg @RemainingArgs
}

Set-Alias -Name vcpkg.exe -Value vcpkg
"@



function New_Vcvenv {
    param (
        [string]$venvName
    )

    $venvPath = Join-Path (Get-Location) $venvName

    # Function to check if a directory is a venv
    function IS_VCVENV ($path) {
        Safe_Path_Exists (Join-Path $path "Scripts\Activate.ps1")
    }

    # Function to prompt user for action
    function Prompt_UserForAction ($path) {
        Write-Host "Directory '$path' already exists."
        if (IS_VCVENV $path) {
            Write-Host "It appears to be a virtual environment."
        } elseif (Get-ChildItem -Path $path -Recurse -Force | Where-Object { -not $_.PSIsContainer }) {
            Write-Host "It contains files and appears to be a random folder with contents."
        } else {
            Write-Host "It is an empty directory."
        }

        $response = Read-Host "Do you want to delete this directory and proceed? (y/N)"
        return $response -eq 'y'
    }

    # Check if the directory exists
    if (Safe_Path_Exists $venvPath) {
        if ((Get-ChildItem -Path $venvPath -Recurse -Force | Measure-Object).Count -eq 0) {
            Write-Host "Directory is empty, proceeding..."
        } else {
            $userWantsToDelete = Prompt_UserForAction $venvPath
            if ($userWantsToDelete) {
                Remove-Item -Path $venvPath -Recurse -Force
                Write-Host "Directory deleted."
            } else {
                Write-Host "Operation cancelled by user."
                exit 0
            }
        }
    }

    New-Item -ItemType Directory -Force -Path $venvPath | Out-Null

    # Attempt to find vcpkg executable
    $vcpkgCmd = "vcpkg"
    $globalVcpkgPath = $null

    try {
        # Get the path of the vcpkg command
        $vcpkgCommand = Get-Command $vcpkgCmd -ErrorAction Stop
        if ($vcpkgCommand) {
            $globalVcpkgPath = Split-Path $vcpkgCommand.Source
            Write-Host "Found vcpkg path at $globalVcpkgPath"
        } else {
            throw "vcpkg not found"
        }
    } catch {
        Write-Host "vcpkg not found. Installing vcpkg globally..."
        $globalVcpkgPath = "$env:ProgramFiles\vcpkg"
        git clone https://github.com/microsoft/vcpkg.git $globalVcpkgPath

        $bootstrapScriptPath = Join-Path -Path $globalVcpkgPath -ChildPath 'bootstrap-vcpkg.bat'
        if (Test-Path -Path $bootstrapScriptPath) {
            Write-Host "Running vcpkg bootstrap script..."
            & $bootstrapScriptPath
        } else {
            Write-Host "vcpkg bootstrap script not found at $bootstrapScriptPath."
            exit 1
        }
    }

    $scriptsPath = Join-Path $venvPath "Scripts"
    New-Item -ItemType Directory -Force -Path $scriptsPath | Out-Null

    $activateScriptPath = Join-Path $scriptsPath "Activate.ps1"
    $activateScriptContent | Out-File -FilePath $activateScriptPath -Encoding UTF8

    Write-Host "C++ virtual environment created at: $venvPath"
    Write-Host "Run 'Scripts\Activate.ps1' to activate it."
}



function Activate_Vcvenv( [string]$venvName ) {
    $curLoc = Get-Location
    $venvPath = Join-Path ($curLoc) $venvName
    if (-not (Safe_Path_Exists $venvPath )) {
        Show_Error_And_Exit "Virtual environment '$venvName' does not exist at '$curLoc'"
    }

    . "$venvPath\Activate.ps1"
}

function deactivate {
    if ($null -ne $env:VCPKG_ROOT) {
        Remove-Item -Path $env:VCPKG_ROOT -Recurse -Force
        [System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", $null, "Process")
        [System.Environment]::SetEnvironmentVariable("Path", $env:Path -replace ";$env:VCPKG_ROOT", "", "Process")
        function prompt { "$(Get-Location)> " }
        Write-Host "Deactivated C++ Vcvenv"
    } else {
        Write-Host "No active C++ virtual environment found."
    }
}

function Manage_Package {
    param (
        [string]$operation,
        [string]$packageName
    )

    if (-not $env:VCPKG_ROOT) {
        Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate."
    }

    if (-not $packageName -and ($operation -eq "install" -or $operation -eq "remove")) {
        Show_Error_And_Exit "Package name is required for $operation command."
    }

    $vcpkgCmd = Join-Path -Path $env:VCPKG_ROOT -ChildPath "vcpkg.exe"

    switch ($operation) {
        "install" { & $vcpkgCmd install $packageName }
        "remove" { & $vcpkgCmd remove $packageName }
        "list" { & $vcpkgCmd list }
        "search" { & $vcpkgCmd search $packageName }
        "upgrade" { & $vcpkgCmd upgrade $packageName }
        default { Show_Error_And_Exit "Unknown operation: $operation" }
    }
}


function Install-From-Requirements {
    param (
        [string]$filePath
    )

    if (-not $env:VCPKG_ROOT) {
        Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate."
    }

    if (-not (Safe_Path_Exists $filePath )) {
        Show_Error_And_Exit "Requirements file '$filePath' does not exist."
    }

    $packages = Get-Content $filePath
    foreach ($package in $packages) {
        Manage_Package -operation "install" -packageName $package
    }
}

function Freeze_Requirements( [string]$filePath = "requirements.txt" ) {
    if (-not $env:VCPKG_ROOT) { Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate." }

    $installedPackages = & "$env:VCPKG_ROOT\vcpkg.exe" list
    $installedPackages | Out-File -FilePath $filePath -Encoding UTF8

    Write-Host "Frozen installed packages to '$filePath'"
}

function Upgrade_Packages {
    if (-not $env:VCPKG_ROOT) { Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate." }

    & "$env:VCPKG_ROOT\vcpkg.exe" upgrade --no-dry-run
    Write-Host "Upgraded all packages to their latest versions"
}

function Uninstall-Vcvenv( [string]$venvName ) {

    $venvPath = Join-Path (Get-Location) $venvName
    if (Safe_Path_Exists $venvPath ) {
        Remove-Item -Path $venvPath -Recurse -Force
        Write-Host "Uninstalled virtual environment '$venvName'"
    } else { Show_Error_And_Exit "Virtual environment '$venvName' does not exist." }
}

function List_All_VENVcs {
    Get-ChildItem -Directory -LiteralPath (Get-Location) | ForEach-Object {
        if (Safe_Path_Exists "$($_.FullName)\vcpkg" ) { Write-Host $_.Name }
    }
}

function Env_Info {
    if (-not $env:VCPKG_ROOT) {
        Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate."
    }

    Write-Host "Environment Path: $env:VCPKG_ROOT"
    & "$env:VCPKG_ROOT\vcpkg.exe" list
}

function Show-Dependencies {
    if (-not $env:VCPKG_ROOT) {
        Show_Error_And_Exit "No active C++ virtual environment found. Please run 'Activate.ps1' to activate."
    }

    & "$env:VCPKG_ROOT\vcpkg.exe" depend-info
}

function Rename-CppVenv {
    param (
        [string]$oldName,
        [string]$newName
    )

    $oldPath = Join-Path (Get-Location) $oldName
    $newPath = Join-Path (Get-Location) $newName

    if (-not (Safe_Path_Exists $oldPath )) {
        Show_Error_And_Exit "Virtual environment '$oldName' does not exist."
    }

    if (Safe_Path_Exists $newPath ) {
        Show_Error_And_Exit "A virtual environment with the name '$newName' already exists."
    }

    Rename-Item -Path $oldPath -NewName $newName
    Write-Host "Renamed virtual environment '$oldName' to '$newName'"
}

function Clone-CppVenv {
    param (
        [string]$srcName,
        [string]$dstName
    )

    $srcPath = Join-Path (Get-Location) $srcName
    $dstPath = Join-Path (Get-Location) $dstName

    if (-not (Safe_Path_Exists $srcPath )) {
        Show_Error_And_Exit "Source virtual environment '$srcName' does not exist."
    }

    if (Safe_Path_Exists $dstPath ) {
        Show_Error_And_Exit "A virtual environment with the name '$dstName' already exists."
    }

    Copy-Item -Recurse -Path $srcPath -Destination $dstPath
    Write-Host "Cloned virtual environment '$srcName' to '$dstName'"
}

function Update-Script {
    $scriptUrl = "https://github.com/th3w1zard1/CTestIFileDialog/vcvenv.ps1"
    $scriptPath = $MyInvocation.MyCommand.Path

    Invoke-WebRequest -Uri $scriptUrl -OutFile $scriptPath
    Write-Host "Script updated to the latest version"
}

try {
    switch ($command) {
        "new" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "new"
            }
            elseif ($subcommand) {
                New_Vcvenv -venvName $subcommand
            }
            else {
                Show_Command_Help "new"
            }
        }
        "activate" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "activate"
            }
            elseif ($subcommand) {
                Activate_Vcvenv -venvName $subcommand
            }
            else {
                Show_Command_Help "activate"
            }
        }
        "deactivate" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "deactivate"
            }
            else {
                deactivate
            }
        }
        "install" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "install"
            }
            elseif ($packageName) {
                Manage_Package -operation "install" -packageName $packageName
            }
            elseif ($requirementsFile) {
                Install-From-Requirements -filePath $requirementsFile
            }
            else {
                Show_Command_Help "install"
            }
        }
        "remove" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "remove"
            }
            elseif ($packageName) {
                Manage_Package -operation "remove" -packageName $packageName
            }
            else {
                Show_Command_Help "remove"
            }
        }
        "list" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "list"
            }
            else {
                Manage_Package -operation "list"
            }
        }
        "search" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "search"
            }
            elseif ($packageName) {
                Manage_Package -operation "search" -packageName $packageName
            }
            else {
                Show_Command_Help "search"
            }
        }
        "freeze" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "freeze"
            }
            else {
                Freeze_Requirements
            }
        }
        "install-requirements" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "install-requirements"
            }
            elseif ($requirementsFile) {
                Install-From-Requirements -filePath $requirementsFile
            }
            else {
                Show_Command_Help "install-requirements"
            }
        }
        "upgrade" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "upgrade"
            }
            elseif ($packageName) {
                Manage_Package -operation "upgrade" -packageName $packageName
            }
            else {
                Upgrade_Packages
            }
        }
        "uninstall-env" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "uninstall-env"
            }
            elseif ($subcommand) {
                Uninstall-Vcvenv -venvName $subcommand
            }
            else {
                Show_Command_Help "uninstall-env"
            }
        }
        "List_All_VENVcs" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "List_All_VENVcs"
            }
            else {
                List_All_VENVcs
            }
        }
        "Env_Info" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "Env_Info"
            }
            else {
                Env_Info
            }
        }
        "dependencies" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "dependencies"
            }
            else {
                Show-Dependencies
            }
        }
        "rename-env" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "rename-env"
            }
            elseif ($subcommand -and $MyInvocation.BoundParameters["packageName"]) {
                Rename-CppVenv -oldName $subcommand -newName $packageName
            }
            else {
                Show_Command_Help "rename-env"
            }
        }
        "clone-env" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "clone-env"
            }
            elseif ($subcommand -and $MyInvocation.BoundParameters["packageName"]) {
                Clone-CppVenv -srcName $subcommand -dstName $packageName
            }
            else {
                Show_Command_Help "clone-env"
            }
        }
        "update-script" {
            if ($MyInvocation.BoundParameters.ContainsKey("subcommand") -and $subcommand.ToLowerInvariant() -eq "--help") {
                Show_Command_Help "update-script"
            }
            else {
                Update-Script
            }
        }
        default {
            Show_Help
        }
    }
} catch {
    Handle_Error $_
}
