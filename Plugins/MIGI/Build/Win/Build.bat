:: Compile all cuda files under the Private/CUDA directory to PTX code.
:: The PTX code will be stored in the Private/PTX directory.

:: Create MSVC Environment.
:: TODO specify the path yourself.
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Set the path to the CUDA compiler.
set CUDACOMPILER="nvcc.exe"
:: Set the path to the PTX directory.
set PTXDIR="%BINARY_OUTPUT_DIR%\PTX"
:: Set the path to the CUDA source directory.
set CUDASRCDIR="Private\CUDA"
:: Create the PTX directory if it does not exists.
if not exist %PTXDIR% mkdir %PTXDIR%
:: Enumerate the CUDA files and compile each of them. If a compilation fails, halt and report.
for /r %CUDASRCDIR% %%f in (*.cu) do (
    %CUDACOMPILER% -ptx -arch=native -o %PTXDIR%\%%~nf.ptx %%f
    if %ERRORLEVEL% NEQ 0 (
        echo Compilation of %%f failed.
        exit /b 1
    )
    echo Compilation of %%f succeeded.
)
exit /b 0
