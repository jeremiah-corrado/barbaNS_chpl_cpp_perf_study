
chpl nsStep7.chpl -o ./build/nsStep7 -screatePlots=true
chpl nsStep8-9.chpl -o ./build/nsStep89 -screatePlots=true

chpl nsStep7.chpl -o ./release/nsStep7 -screatePlots=false --fast
chpl nsStep8-9.chpl -o ./release/nsStep89 -screatePlots=false -stermOnTol=false --fast
