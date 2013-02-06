# bash completion for wissbi

_wissbi ()
{
    local cur=${COMP_WORDS[COMP_CWORD]}
    COMPREPLY=( $( compgen -W "`ls /var/lib/wissbi/sub`" -- $cur) )
}
complete -F _wissbi wissbi-sub
complete -F _wissbi wissbi-pub

# Local variables:
# mode: shell-script
# sh-basic-offset: 4
# sh-indent-comment: t
# indent-tabs-mode: nil
# End:
# ex: ts=4 sw=4 et filetype=sh
