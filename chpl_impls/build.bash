if [[ $# -eq 0 ]]; then
    echo "building chpl source ..."
    chpl nsStep7.chpl -o ./build/nsStep7 -screatePlots=true
    chpl nsStep8-9.chpl -o ./build/nsStep89 -screatePlots=true
else
    echo "building chpl source with --fast ..."
    chpl nsStep7.chpl -o ./release/nsStep7 -screatePlots=false --fast
    chpl nsStep8-9.chpl -o ./release/nsStep89 -screatePlots=false -stermOnTol=false --fast
fi
