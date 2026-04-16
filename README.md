# PythonHelper
An Unreal Engine plugin that allows you to execute custom editor scripts with a single click.


![image](https://github.com/user-attachments/assets/9e453bc3-026b-46a1-8872-4ad14a30d2fe)

## Features
1. This plug-in provides an "unreallib.py" module, which contains a complete UnrealEngine Python API. When you use it in text editors such as VS Code, you will get intelligent code prompt, function parameter prompt, return value type inference, comments, etc. you can also jump to the function in the module to view the detailed function definition.

2. Automatically detect all Python files in the "$ProjectDir$/ Python" directory and run the file by clicking a button.

3. After clicking the python file, it will automatically identify the functions defined and call them directly by clicking a button (only functions without parameters can be executed)

4. When you edit a file in VS Code, it will automatically detect the change of the file and refresh the list of callable functions. The automatic refresh function can be turned off. 
Note: when "import" a module for the first time or  through "importlib.reload ()" to reloads a module, it executes the entire module.

5. You can directly open the python folder of the current work by shortcut.

6. When the panel is closed, all automatic detection will be closed until the panel is reopened.

## Startup

1. Open PythonHelper Tab( MainMenu -> Window -> PythonHelper)  

![screenshot-20220302-132219](https://user-images.githubusercontent.com/51103322/156300572-9694e6fe-98d8-479f-b504-d3b2b29474fd.png)  

## Known Issue






## 中文

一键执行编辑器 Python 脚本

## 设置

1. 打开面板( 主菜单 -> Window -> PythonHelper)  

![screenshot-20220302-132219](https://user-images.githubusercontent.com/51103322/156300572-9694e6fe-98d8-479f-b504-d3b2b29474fd.png)  

## 插件功能

1. 本插件提供一个 unreallib.py 模块，模块中包含了完整的 UnrealEngine Python API, 当你在VS Code 等编辑器中使用的时候，将获得智能代码提示，函数参数提示，返回值类型推断，注释等，也能跳转到模块中的函数，查看详细的函数定义。

2. 自动检测 "$ProjectDir$/ Python" 目录下的所有 python 文件，通过点击按钮运行该文件

3. 点选Python文件后，会自动识别文件中定义的函数，通过点击按钮直接调用（只能执行无参数的函数）

4. 当你在VS Code 中编辑文件的时候，自动检测文件的变动，并且重新刷新可调用函数列表。自动刷新功能可以关闭。注意：在第一次 import 模块的时候，和通过 importlib.reload() 重新加载模块的时候，会执行整个模块。

5. 可以通过快捷方式，直接打开当前工作的Python文件夹

6. 当面板关闭的时候，所有的自动检测将会关闭，直到重新打开面板

## 已知问题
