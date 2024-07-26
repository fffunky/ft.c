# the 'ft' command

ft.c is my own implementation of the 'ls' command written in C. in order to use it simply `git clone` this project wherever you'd like in your system and create an alias in your `.bashrc`/`.zshrc` that points to the `./ft` executable file:
```sh
alias ft='path/to/ft'
```

# Using 'ft'

When you write `ft` in the terminal (without any args), you should see an output that looks like this:
![a terminal running the ft command and outputting a list of 3 directories, an executable file and 2 regular files, color coded to distinguish these types](./assets/ft_example.png "using ft without arguments")

You can pass a directory path to `ft` in order to output the files in that directory as follows:
![a terminal running the ft command with an argument directory_2, which outputs the contents of the directory_2 folder](./assets/ft_args_example.png "using ft with arguments")

TODO:
 - add 'ls -a' functionality where dotfiles are shown as well.
