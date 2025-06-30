# elf_analysistool/views.py
from django.shortcuts import render
from django.core.files.storage import default_storage
import subprocess
import os

def upload_and_analyze(request):
    result = None
    if request.method == 'POST' and 'elf_file' in request.FILES:
        file = request.FILES['elf_file']
        file_path = default_storage.save('uploads/' + file.name, file)

        # 调用你编写的 C/C++ ELF 分析程序
        exe_path = os.path.abspath('../elf_analyzer/build/elf-analyzer.exe')  # ELF分析器可执行文件
        full_path = os.path.abspath(file_path)

        try:
            output = subprocess.check_output([exe_path, full_path], text=True)
            result = output
        except subprocess.CalledProcessError as e:
            result = f"分析失败：{e}"

    return render(request, 'upload.html', {'result': result})
