## Summary

🍺 /opt/homebrew/Cellar/llvm@13/13.0.1: 5,449 files, 871.5MB

## Running `brew cleanup llvm@13`...

Disable this behaviour by setting HOMEBREW_NO_INSTALL_CLEANUP.
Hide these hints with HOMEBREW_NO_ENV_HINTS (see `man brew`).

### Caveats

### llvm@13

To use the bundled libc++ please add the following LDFLAGS:
LDFLAGS="-L/opt/homebrew/opt/llvm@13/lib -Wl,-rpath,/opt/homebrew/opt/llvm@13/lib"

llvm@13 is keg-only, which means it was not symlinked into /opt/homebrew,
because this is an alternate version of another formula.

If you need to have llvm@13 first in your PATH, run:
echo 'export PATH="/opt/homebrew/opt/llvm@13/bin:$PATH"' >> ~/.zshrc

For compilers to find llvm@13 you may need to set:
export LDFLAGS="-L/opt/homebrew/opt/llvm@13/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm@13/include"
