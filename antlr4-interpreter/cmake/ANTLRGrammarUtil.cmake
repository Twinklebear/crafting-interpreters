find_package(Java QUIET)
find_file(ANTLR_GRAMMAR_UTIL_SH NAME ANTLRGrammarUtil.sh.in
    PATHS
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_LIST_DIR})

function(antlr_grammar_util TargetName GrammarFile)
    get_filename_component(ANTLR_INPUT ${GrammarFile} NAME_WE)

    set(OUTPUT_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/antlr4java_generated_src/${ANTLR_INPUT})

    unset(JAVA_SRC)
    list(APPEND JAVA_SRC
        ${OUTPUT_DIR}/${ANTLR_INPUT}Lexer.java
        ${OUTPUT_DIR}/${ANTLR_INPUT}Parser.java)

    unset(ANTLR_OUTPUTS)
    list(APPEND ANTLR_OUTPUTS
        ${OUTPUT_DIR}/${ANTLR_INPUT}.interp
        ${OUTPUT_DIR}/${ANTLR_INPUT}.tokens
        ${OUTPUT_DIR}/${ANTLR_INPUT}Lexer.interp
        ${OUTPUT_DIR}/${ANTLR_INPUT}Lexer.tokens
        ${JAVA_SRC})

    add_custom_command(
        OUTPUT ${ANTLR_OUTPUTS}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
                ${GrammarFile}
                -o ${OUTPUT_DIR}
                -no-listener
                -no-visitor
        DEPENDS ${InputFile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building Grammar ${GrammarFile}")

    list(APPEND JAVA_CLASSES
        ${OUTPUT_DIR}/${ANTLR_INPUT}Lexer.class
        ${OUTPUT_DIR}/${ANTLR_INPUT}Parser.class)

    add_custom_command(
        OUTPUT ${JAVA_CLASSES}
        COMMAND ${Java_JAVAC_EXECUTABLE} ${JAVA_SRC}
        DEPENDS ${JAVA_SRC}
        WORKING_DIRECTORY ${OUTPUT_DIR}
        COMMENT "Compiling ${ANTLR_INPUT} Java Files")

    get_filename_component(ANTLR_DIR ${ANTLR_EXECUTABLE} DIRECTORY)

    configure_file(${ANTLR_GRAMMAR_UTIL_SH} ${TargetName}.sh @ONLY)

    add_custom_target(${TargetName} ALL DEPENDS ${JAVA_CLASSES} ${TargetName}.sh)
endfunction()

