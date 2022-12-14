set(WEASEL_LIBS
    ${WEASEL_LIBS}
    # AMD GPU
    LLVMAMDGPUInfo
    LLVMAMDGPUAsmParser
    LLVMAMDGPUCodeGen
    LLVMAMDGPUDesc
    LLVMAMDGPUDisassembler
    LLVMAMDGPUUtils

    # ARCH64
    LLVMAArch64Info

    LLVMAArch64AsmParser
    LLVMAArch64CodeGen
    LLVMAArch64Desc
    LLVMAArch64Disassembler
    LLVMAArch64Utils

    # ARM INFO
    LLVMARMInfo
    LLVMARMAsmParser
    LLVMARMCodeGen
    LLVMARMDesc
    LLVMARMDisassembler
    LLVMARMUtils

    # AVR
    LLVMAVRInfo
    LLVMAVRAsmParser
    LLVMAVRCodeGen
    LLVMAVRDesc
    LLVMAVRDisassembler
    # LLVMAVRUtils

    # BPF
    LLVMBPFInfo
    LLVMBPFAsmParser
    LLVMBPFCodeGen
    LLVMBPFDesc
    LLVMBPFDisassembler
    # LLVMBPFUtils

    # Hexagon
    LLVMHexagonInfo
    LLVMHexagonAsmParser
    LLVMHexagonCodeGen
    LLVMHexagonDesc
    LLVMHexagonDisassembler
    # LLVMHexagonUtils

    # Lanai
    LLVMLanaiInfo
    LLVMLanaiAsmParser
    LLVMLanaiCodeGen
    LLVMLanaiDesc
    LLVMLanaiDisassembler
    # LLVMLanaiUtils

    # MIPS
    LLVMMipsInfo
    LLVMMipsAsmParser
    LLVMMipsCodeGen
    LLVMMipsDesc
    LLVMMipsDisassembler
    # LLVMMipsUtils

    # MSP430
    LLVMMSP430Info
    LLVMMSP430AsmParser
    LLVMMSP430CodeGen
    LLVMMSP430Desc
    LLVMMSP430Disassembler
    # LLVMMSP430Utils

    # NVPTX
    LLVMNVPTXInfo
    # LLVMNVPTXAsmParser
    LLVMNVPTXCodeGen
    LLVMNVPTXDesc
    # LLVMNVPTXDisassembler
    # LLVMNVPTXUtils

    # Power PC
    LLVMPowerPCInfo
    LLVMPowerPCAsmParser
    LLVMPowerPCCodeGen
    LLVMPowerPCDesc
    LLVMPowerPCDisassembler
    # LLVMPowerPCUtils

    # RISC V
    LLVMRISCVInfo
    LLVMRISCVAsmParser
    LLVMRISCVCodeGen
    LLVMRISCVDesc
    LLVMRISCVDisassembler

    # Sparc
    LLVMSparcInfo
    LLVMSparcAsmParser
    LLVMSparcCodeGen
    LLVMSparcDesc
    LLVMSparcDisassembler
    # LLVMSparcUtils

    # System Z
    LLVMSystemZInfo
    LLVMSystemZAsmParser
    LLVMSystemZCodeGen
    LLVMSystemZDesc
    LLVMSystemZDisassembler
    # LLVMSystemZUtils

    # Web Assembly
    LLVMWebAssemblyInfo
    LLVMWebAssemblyAsmParser
    LLVMWebAssemblyCodeGen
    LLVMWebAssemblyDesc
    LLVMWebAssemblyDisassembler
    # LLVMWebAssemblyUtils

    # X86
    LLVMX86Info
    LLVMX86AsmParser
    LLVMX86CodeGen
    LLVMX86Desc
    LLVMX86Disassembler
    # LLVMX86Utils

    # Core
    LLVMXCoreInfo
    # LLVMXCoreAsmParser
    # LLVMXCoreCodeGen
    # LLVMXCoreDesc
    # LLVMXCoreDisassembler
    # LLVMXCoreUtils
)
