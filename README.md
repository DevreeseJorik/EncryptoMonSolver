# EncryptoMonSolver

EncryptoMonSolver is designed to perform a brute force search for a specific sequence of data within an encrypted Pokémon's data. The tool is compatible with the commonly used .pkm format, utilized by applications such as Pokegen, PkHex, and similar tools for storing Pokémon data.

# IMPORTANT

Attention: This codebase has recently been created, and it is currently in the process of being properly established. Kindly refrain from utilizing it until it has undergone thorough development and validation.

# Features
The tool tweaks easy to modify data within the provided Pokémon's data, such as held item, experience points, moves, evs,...
The Pokémon is then encrypted, and a search for a specific sequence of data is performed. It takes a brute force approach,
attempting combinations of these tweaked settings until it either matches the requested sequence or cannot find any solution.

# Use Case
EncryptoMonSolver was developed with a focus on Arbitrary Code Execution practices. It is meant to use in scenarios where an encrypted Pokémon's
data can be read as script or assembly instructions. Currently the tool does not support Battle Stats, as the most common scenario to read a Pokémon's
data is in Box Data, which does not store Battle Stats.

# How to use EncryptoMonSolver

# Clone the Repository:
Clone the EncryptoMonSolver repository to your local machine using the following command:

```bash
git clone https://github.com/DevreeseJorik/EncryptoMonSolver.git
```

Navigate to the Repository:
Change your current directory to the newly cloned EncryptoMonSolver repository:

```bash
cd EncryptoMonSolver
```

Compile the Code:
Run the provided Makefile to compile the source code and generate the executable. Execute the following command:

```bash
make
```

The encryptomon.exe executable can be found in `/output` along with temporary compilation files in `/output/temp`.

Cleanup (Optional):
If desired, you can remove temporary compilation files using the following command:

```bash
make clean
```

# Run EncryptoMonSolver

Prepare Input:
Place your poke.bin file, formatted as a .pkm file, in the same directory as the encryptomon.exe executable.
You may use Pkhex or Pokegen to extract a Pokémon from a save file to use, just rename it to poke.bin. 

TODO: include ingame trade Pokémon, which have static data.

Run the Tool:
Execute the EncryptoMonSolver by running the following command:

```bash
./output/encryptomon.exe
```

A directory will be generated containing the tweaked Pokémon and encrypted form which contains the requested sequence.
