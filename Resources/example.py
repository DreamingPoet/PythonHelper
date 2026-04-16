# Copyright 2022 Xinin Zeng (DreamingPoet). All Rights Reserved.

try:
    import unreal
except:
    from unreallib import unreallib as unreal

import sys
import os


class MyObj():
    def __init__(self):
        self.name = "MyObj"
        pass

    def hello(self):
        print(self.name + ":" + "Hello Python!")

# Only module functions without parameters can be 
# recognized by PythonHelper plugin.
def say_hello():
    a = MyObj()
    a.hello()

def say_nihao(): ##alias=你好##
    print("你好!")


def say_annyonghaseyo(): ##alias=안녕하세요##
    print("안녕하세요!")
    

def say_ohayougozaimasu(): ##alias=おはようございます##
    print("おはようございます!")

# show python version of the current UnrealEngine
def get_python_version():
    print(sys.version)

# spawn actor
def spawn_actor():
    _class = unreal.EditorAssetLibrary.load_blueprint_class(
        '/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialCharacter.TutorialCharacter'
    )
    location = unreal.Vector(0, 0, 90)
    rotation = unreal.Rotator(0, 0, -70)
    unreal.EditorLevelLibrary.spawn_actor_from_class(_class, location,rotation)
    location = unreal.Vector(0, 150, 90)
    rotation = unreal.Rotator(0, 0, -160)
    unreal.EditorLevelLibrary.spawn_actor_from_class(_class, location,rotation)
    location = unreal.Vector(150, 150, 90)
    rotation = unreal.Rotator(0, 0, 110)
    unreal.EditorLevelLibrary.spawn_actor_from_class(_class, location,rotation)


# destroy actor
def destroy_actor():
    
    a = unreal.EditorLevelLibrary.get_all_level_actors()
    # declare “i” type so we can get intelligent code prompt 声明 i 的对象类型，以便通过 i. 来获得代码提示
    # i:unreal.Actor 
    for i in a:
        if("TutorialCharacter" in i.get_full_name()):
            # print(i.get_full_name())
            i.destroy_actor()


#
def delete_example_file():

    user_confirmation = unreal.EditorDialog.show_message(
    'Are you sure?',
    'example.py will be deleted and cannot be retrieved!',
    unreal.AppMsgType.YES_NO,
    unreal.AppReturnType.NO
    )
    
    if user_confirmation == unreal.AppReturnType.YES:
        # delete source file of example.py 
        example_file = unreal.Paths.engine_plugins_dir() + "/Marketplace/PythonHelper/Resources/example.py"
        if os.path.exists(example_file):
            os.remove(example_file)

        project_plugins_dir = unreal.Paths.project_plugins_dir()
        if os.path.exists(project_plugins_dir):
            for dir in os.listdir(project_plugins_dir):
                if os.path.exists(project_plugins_dir + "/" + dir + "/PythonHelper.uplugin"):
                    example_file = project_plugins_dir + "/" + dir + "/Resources/example.py"
                    if os.path.exists(example_file):
                        os.remove(example_file)
        
        # delete current example.py file
        example_file = os.path.abspath(__file__)
        if os.path.exists(example_file):
            unreal.log_warning("example.py deleted!")
            os.remove(example_file)

    if user_confirmation == unreal.AppReturnType.NO:
        # cancell all items
        return