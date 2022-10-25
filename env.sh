# In zsh the value of $0 depends on the FUNCTION_ARGZERO option which is
# set by default. FUNCTION_ARGZERO, when it is set, sets $0 temporarily
# to the name of the function/script when executing a shell function or
# sourcing a script. POSIX_ARGZERO option, when it is set, exposes the
# original value of $0 in spite of the current FUNCTION_ARGZERO setting.
#
# Note: The version of zsh need to be 5.0.6 or above. Any versions below
# 5.0.6 maybe encoutner errors when sourcing this script.
if [ -n "${ZSH_VERSION:-}" ]; then
	dir="${(%):-%N}"
	if [ $options[posixargzero] != "on" ]; then
		setopt posixargzero
		name=$(basename -- "$0")
		unsetopt posixargzero
	else
		name=$(basename -- "$0")
	fi
else
	dir="${BASH_SOURCE[0]}"
	name=$(basename -- "$0")
fi

if [ "X$name" "==" "Xenv.sh" ]; then
    echo "Do not source this file to set the System environment."
    exit
fi

# You can further customize your environment by creating a bash script called
# .zephyrrc in your home directory. It will be automatically
# run (if it exists) by this script.

if uname | grep -q "MINGW"; then
    win_build=1
    pwd_opt="-W"
else
    win_build=0
    pwd_opt=""
fi

# identify OS source tree root directory
export SDK_BASE=$( builtin cd "$( dirname "$dir" )" > /dev/null && pwd ${pwd_opt})
unset pwd_opt
