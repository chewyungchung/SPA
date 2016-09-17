Instructions:

1. Build every project in win32 configuration.
2. Please include dependencies (SPA.lib)


Bug fixes:
1. Removed access violation (vector.at(i)) that doesnt check for boundary conditions
2. Fix pattern query evaluation
3. Removed all static pointers as we don't know how to handle it