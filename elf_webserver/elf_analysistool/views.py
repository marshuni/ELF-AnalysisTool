# elf_analysistool/views.py
from django.shortcuts import render
from django.core.files.storage import default_storage
import os
import subprocess
import json

def hexify(val, width=8):
    if isinstance(val, int):
        return f"0x{val:0{width}X}"
    return val

def format_size(val):
    if isinstance(val, int):
        return f"{val} 字节"
    return val

# ELF 头格式化
def explain_elf_class(val):
    return {
        0: "无效",
        1: "32位 (ELF32)",
        2: "64位 (ELF64)"
    }.get(val, f"未知({val})")
def explain_data_encoding(val):
    return {
        0: "无效",
        1: "小端 (Little Endian)",
        2: "大端 (Big Endian)"
    }.get(val, f"未知({val})")
def explain_os_abi(val):
    table = {
        0: "System V",
        1: "HP-UX",
        2: "NetBSD",
        3: "Linux",
        6: "Solaris",
        7: "AIX",
        8: "IRIX",
        9: "FreeBSD",
        10: "Tru64",
        11: "Novell Modesto",
        12: "OpenBSD",
        13: "OpenVMS",
        14: "NonStop Kernel",
        15: "AROS",
        16: "Fenix OS",
        17: "CloudABI",
        18: "Stratus Technologies OpenVOS",
    }
    return table.get(val, f"未知({val})")
def explain_type(val):
    table = {
        0: "无类型 (ET_NONE)",
        1: "可重定位文件 (ET_REL)",
        2: "可执行文件 (ET_EXEC)",
        3: "共享目标文件 (ET_DYN)",
        4: "核心转储文件 (ET_CORE)"
    }
    return table.get(val, f"未知({val})")
def explain_machine(val):
    table = {
        0: "No specific instruction set",
        3: "x86",
        40: "ARM",
        62: "x86-64",
        183: "AArch64",
        243: "RISC-V"
    }
    return table.get(val, f"未知({val})")
def format_elf_header(raw):
    # 格式化 ELF Header
    elf_header = raw.get("elf_header", {}).copy()
    # 解释字段
    elf_header["class"] = explain_elf_class(elf_header["class"])
    elf_header["data_encoding"] = explain_data_encoding(elf_header["data_encoding"])
    elf_header["os_abi"] = explain_os_abi(elf_header["os_abi"])
    elf_header["type"] = explain_type(elf_header["type"])
    elf_header["machine"] = explain_machine(elf_header["machine"])
    # 地址类字段十六进制
    for key in ["entry_point", "program_header_offset", "section_header_offset"]:
        if key in elf_header:
            elf_header[key] = hexify(elf_header[key])
    # 大小/字节数字段加单位
    for key in ["header_size", "program_header_entry_size", "section_header_entry_size"]:
        if key in elf_header:
            elf_header[key] = format_size(elf_header[key])
    # 其余字段十六进制（如flags）
    if "flags" in elf_header:
        elf_header["flags"] = hexify(elf_header["flags"],2)
    return elf_header

# 节头表格式化
SECTION_TYPE_MAP = {
    0: "NULL（无效节）",
    1: "PROGBITS（程序数据）",
    2: "SYMTAB（符号表）",
    3: "STRTAB（字符串表）",
    4: "RELA（重定位表，带Addend）",
    5: "HASH（哈希表）",
    6: "DYNAMIC（动态链接信息）",
    7: "NOTE（备注）",
    8: "NOBITS（未分配空间）",
    9: "REL（重定位表，无Addend）",
    10: "SHLIB（保留）",
    11: "DYNSYM（动态符号表）",
    14: "INIT_ARRAY（构造函数指针数组）",
    15: "FINI_ARRAY（析构函数指针数组）",
    16: "PREINIT_ARRAY（预初始化函数指针数组）",
    17: "GROUP（节组）",
    18: "SYMTAB_SHNDX（节索引扩展表）",
    # ...可补充更多类型...
}
def explain_section_type(val):
    # 兼容字符串和数字
    try:
        val_int = int(val)
    except Exception:
        return f"未知({val})"
    return SECTION_TYPE_MAP.get(val_int, f"{val}（未知）")
def explain_section_flags(val):
    # ELF 标准节标志位
    flags = []
    if isinstance(val, int):
        if val & 0x1:
            flags.append("W(可写)")
        if val & 0x2:
            flags.append("A(分配)")
        if val & 0x4:
            flags.append("X(可执行)")
        if val & 0x10:
            flags.append("M(合并)")
        if val & 0x20:
            flags.append("S(字符串表)")
        if val & 0x40:
            flags.append("I(信息链接)")
        if val & 0x80:
            flags.append("L(保留)")
        if val & 0x100:
            flags.append("O(保留)")
        if val & 0x200:
            flags.append("G(组)")
        if val & 0x400:
            flags.append("T(线程本地存储)")
        if val & 0x800:
            flags.append("C(压缩)")
    return " | ".join(flags) if flags else f"0"
def format_section_headers(raw):
    # 格式化 Section Headers
    section_headers = raw.get("section_headers", [])
    formatted = []
    for sh in section_headers:
        item = {
            "序号": sh.get("index"),
            "名称": sh.get("name"),
            "类型": explain_section_type(sh.get("type")),
            "地址": hexify(sh.get("addr")) if "addr" in sh else "",
            "偏移": hexify(sh.get("offset")) if "offset" in sh else "",
            "大小": format_size(sh.get("size")) if "size" in sh else "",
            "标志": explain_section_flags(sh.get("flags")) if "flags" in sh else "",
            "标志原始值": hexify(sh.get('flags'),4) if "flags" in sh else "",
            "索引": sh.get("link"),
            "附加信息": sh.get("info"),
            "内存对齐": format_size(sh.get("addralign")) if "addralign" in sh else "",
        }
        formatted.append(item)
    return formatted

# 符号表格式化
def explain_symbol_type(val):
    # 提取出符号类型位
    val = val & 0xF  # 只保留低4位
    # ELF 符号类型
    types = {
        0: "未指定",
        1: "数据对象 (STT_OBJECT)",
        2: "函数 (STT_FUNC)",
        3: "节 (STT_SECTION)",
        4: "文件 (STT_FILE)",
    }
    return types.get(val, f"未知({val})")
def explain_symbol_binding(val):
    # 提取出符号绑定类型位
    val = val >> 4
    # ELF 符号绑定类型
    bindings = {
        0: "本地 (STB_LOCAL)",
        1: "全局 (STB_GLOBAL)",
        2: "弱 (STB_WEAK)",
    }
    return bindings.get(val, f"未知({val})")
def format_symbols(raw):
    symbols = raw.get("symbols", [])
    formatted = []
    for section in symbols:
        section_name = section.get("name", "<unknown>")
        section_symbols = section.get("symbols", [])
        symtab_list = []
        for s in section_symbols:
            symtab = {
                "序号": s.get("index"),
                "名称": s.get("name"),
                "值": hexify(s.get("value", 0),4),
                "大小": s.get("size"),
                "符号类型": explain_symbol_type(s.get("info", -1)),
                "绑定类型": explain_symbol_binding(s.get("info", -1)),
                "所在节": s.get("shndx"),
            }
            symtab_list.append(symtab)
        item = {
            "节名称": section_name,
            "符号表": symtab_list,
        }
        formatted.append(item)
    return formatted

def format_dynamic_segment(raw):
    dynamic = raw.get("dynamic_segment", [])
    formatted = []
    for d in dynamic:
        item = {
            "Tag": hex(d.get("tag", 0)),
            "值_地址": hex(d.get("val_ptr", 0)),
        }
        formatted.append(item)
    return formatted

def format_relocations(raw):
    relocations = raw.get("relocations", [])
    formatted = []
    for r in relocations:
        item = {
            "偏移": hex(r.get("offset", 0)),
            "Info": hex(r.get("info", 0)),
            "加数": r.get("addend"),
        }
        formatted.append(item)
    return formatted

def format_program_headers(raw):
    # 格式化 Program Headers
    program_headers = raw.get("program_headers", [])
    for ph in program_headers:
        for key in ["offset", "vaddr", "paddr"]:
            if key in ph:
                ph[key] = hexify(ph[key])
        for key in ["filesz", "memsz"]:
            if key in ph:
                ph[key] = format_size(ph[key])
        for key in ["flags", "align"]:
            if key in ph:
                ph[key] = hexify(ph[key],4)
    return program_headers



def format_result(raw):
    return {
        "elf_header": format_elf_header(raw),
        "program_headers": format_program_headers(raw),
        "section_headers": format_section_headers(raw),
        "symbols": format_symbols(raw),
        "dynamic_segment": format_dynamic_segment(raw),
        "relocations": format_relocations(raw),
    }

def index(request):
    result = None
    if request.method == 'GET':
        return render(request, 'start.html')  # 返回一个html页面
    elif request.method == 'POST' and 'elf_file' in request.FILES:
        file = request.FILES['elf_file']
        input_path = default_storage.save('uploads/' + file.name, file)
        input_abs = os.path.abspath(input_path)

        output_json_path = input_abs + '.json'

        # 运行解析器
        exe_path = os.path.abspath('../elf_analyzer/build/elf-analyzer.exe')
        try:
            subprocess.run([exe_path, input_abs, output_json_path], check=True)

            with open(output_json_path, 'r', encoding='utf-8') as f:
                json_data = json.load(f)

            result = format_result(json_data)

        except subprocess.CalledProcessError as e:
            result = {"elf_header": {"error": f"解析器执行失败：{str(e)}"}}
    return render(request, 'index.html', 
                  {
                      'result': result,
                      'file_name': file.name if file else None,
                      })

def about(request):
    return render(request, 'about.html', {})