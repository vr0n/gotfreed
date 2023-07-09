# Targets

These are some example targets I threw together to start a manual PoC of what I wanted to do.

If you want to manually do what I am doing in code, you are probably looking for `example_2`.

Just run `make example_2` or `make all` to build it.

# Walkthrough

These are very weak examples, but they demonstrate the concept well enough.

Using the `example_2` binary, just run it and get its `pid` (can be done with `./example_2` followed by `ps aux | grep -i example_2` or many other ways).

Attach to the process using gdb with `gef`. This can be done with `gdb -p <pid from last step>`. You may get an error crying about not being able to attache to the process. If you see this, the *most likely* (but not necessarily) issue is that you need to run the following:

```
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
```

This **lowers** the security of your system by allowing any process to ptrace attach to any other process, so this is not recommended if you are using your host for this stupid example code.

Now you have stopped the process which was already running so all of the `got` relocations are resolved. 

In this example, the only reasonable `got` entry to overwrite is `fflush`, so type `got` in `gef` to get the address of the pointer to `fflush`.

When you have that, you also need the address of `false_func` (you can get this several ways, but `disas false_func` works).

The idea is to set the value that is pointed to by the `got` entry for `fflush` with the address of `false_func`. 

What this means is that the next time `fflush` is called, the `got` will tell the code to jump to `false_func`.

Because this is just example code, it doesn't handle anything gracefully and throws an intentional exit with a weird code (123). If everything works, you should see the return value of `example_2` equaling 123 and you should also notice that the final entry in `log.file` is "Injected log".

# Note About `example_3`

The final draft is being tested on `example_3` only, so if I break something that makes the previous walkthrough stop working, I apologize, but it very likely won't be fixed. 
