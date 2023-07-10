# To Build

```
mkdir build && cd build
cmake ../
make
```

# To Do

- [ ] Add ability to restore code cave (in case it is not a code cave)
- [ ] Can we grab the symbols for the GOT entries?
- [X] Break out GOT functions to separate lib
- [X] Let user select GOT entry at runtime instead of compile time
- [X] Handle code cave
- [X] Handle overwrite of GOT addr to code cave
- [X] Dynamically grab baseaddr of ELF
- [X] Finish GOT hashmap
