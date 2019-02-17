set(COMPONENT_SRCDIRS   
    "." 
    "ai"
    "cinematics"
    "editor"
#    "platform"
    "scale2x"
    "sdl"
    "tileattr"
    )
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_PRIV_REQUIRES SDL zlib newlib )



register_component()

component_compile_options(
    -Wno-unused-variable 
    -Wno-error=maybe-uninitialized 
    -Wno-missing-field-initializers 
    -Wno-maybe-uninitialized 
    -Wno-ignored-qualifiers 
    -Wno-error=switch
    -Wno-error=char-subscripts
    -Wno-error=pointer-sign
    -Wno-error=format=
    -Wno-error=parentheses
    -Wno-error=unused-value
    -Wno-error=return-type
    -Wno-error=implicit-int
    -Wno-error=unused-label
    -Wno-incompatible-pointer-types
    -Wno-int-conversion
    -Wno-pointer-sign
    )