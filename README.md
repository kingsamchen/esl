# esl

A collection of header-only utilities to be used as extensions to the C++ standard library.

## Setup Git Hooks

First install llvm/clang tools on your operating system, then configure the repo:

```shell
$ cd esl
$ git config core.hooksPath ".githooks"
```

To ensure hook scripts will be executed properly, may require some extra setup on your system.

### clang-format-diff.py

Check against clang-format for code changes.

##### Linux

```bash
{
  # Change at your will.
  llvm_version=15
  src="/usr/share/clang/clang-format-${llvm_version}/clang-format-diff.py"
  target="/usr/bin/clang-format-diff.py"
  command -v "clang-format-diff.py" > /dev/null 2>&1 &&
    echo "clang-format-diff.py is already set" ||
    [[ -s "${src}" ]] &&
      sudo update-alternatives --install ${target} clang-format-diff.py ${src} ${llvm_version}0 ||
      echo "${src} does not exist"
}
```

##### Windows

```powershell
&{
  $llvm_share="path\to\folder\contains\clang-format-diff.py";
  (Get-Command "clang-format-diff.py" 2>&1 > $null) &&
    (echo "clang-format-diff.py is already set") ||
    ((Get-Item -Path "$llvm_share\clang-format-diff.py" > $null) &&
     ([Environment]::SetEnvironmentVariable('Path', $llvm_share + ";" + [Environment]::GetEnvironmentVariable('Path', 'User'), 'User')))
}
```

## License

esl is licensed under the terms of the MIT license. see [LICENSE](https://github.com/kingsamchen/esl/blob/master/LICENSE)
