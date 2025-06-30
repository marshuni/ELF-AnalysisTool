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

def format_result(raw):
    # 格式化 ELF Header
    elf_header = raw.get("elf_header", {})
    for key in ["entry_point", "program_header_offset", "section_header_offset", "flags", "header_size"]:
        if key in elf_header:
            elf_header[key] = hexify(elf_header[key])

    # 格式化 Program Headers
    program_headers = raw.get("program_headers", [])
    for ph in program_headers:
        for key in ["offset", "vaddr", "paddr", "filesz", "memsz", "flags", "align"]:
            if key in ph:
                ph[key] = hexify(ph[key])

    # 格式化 Section Headers
    section_headers = raw.get("section_headers", [])
    for sh in section_headers:
        for key in ["addr", "offset", "size", "flags"]:
            if key in sh:
                sh[key] = hexify(sh[key])

    return {
        "elf_header": elf_header,
        "program_headers": program_headers,
        "section_headers": section_headers
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