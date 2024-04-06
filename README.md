# Work-in-progress Cyberpunk 2077 save file reading implementation
A RED4ext plugin native code implementation of parts of [WolvenKit](https://github.com/WolvenKit/WolvenKit)'s Cyberpunk 2077 save parsing. 
Dependencies include:
- [lz4](https://github.com/lz4/lz4) for save data decompression
- [RED4ext.SDK](https://github.com/WopsS/RED4ext.SDK) for various types utilized by Cyberpunk 2077, taking advantage of Cyberpunk 2077's runtime type information system and plugin initialization
- [RedLib](https://github.com/psiberx/cp2077-red-lib) for a simplified way of registering new native classes within Cyberpunk 2077's scripting system
- [JSON for modern C++](https://github.com/nlohmann/json) for JSON parsing

## Supports
- Player inventory parsing
- Scriptable system parsing
- Minor implementation of PersistencySystem parsing, being able to parse the player's vehicles

## Versions tested on
- 2.12
- Generally should work on anything that's 2.00 or higher, backwards compatibility to 1.63 and lower is irrelevant for the usecase

## To do
- Code cleanup
    - A more general RTTI class reader architecture, being able to overload a base reader class without creating a new namespace for it
    - Making everything share a naming convention 
- More detailed persistency system parsing (At the moment we're only able to parse one class), though I am not sure that is worth the time, given the performance cost of parsing 400 000 classes
- Stats system parsing, but I am unsure about it being worthwhile

## Credits
- The [WolvenKit](https://github.com/WolvenKit/WolvenKit) team, [Seberoth](https://github.com/seberoth) in particular
- [psiberx](https://github.com/psiberx) for help with RED4ext and understanding Cyberpunk 2077's inner workings