import os
import shutil
import platform
from pathlib import Path
import multiprocessing

cpu_count = multiprocessing.cpu_count()
project_dir = os.path.abspath(os.path.dirname(__file__))

def setEnv(name, value):
    if platform.system() == 'Windows':
        os.system(f'set {name}={value}')
    else:
        os.system(f'export {name}={value}')
    os.environ[str(name)] = str(value)

def safeRemove(rm_path):
    try:
        if os.path.isfile(rm_path):
            os.remove(rm_path)
        else:
            shutil.rmtree(rm_path)
    except:
        pass

def _generateCMakeProject(target, arch):
    cmake_cmd = f'cmake {project_dir} -DCMAKE_BUILD_TYPE=Release '
    if target == "windows":
        if arch == "x86":
            cmake_cmd += f' -A Win32'
        else:
            cmake_cmd += f' -A X64'
    elif target == "android":
        toolchain = Path(os.environ.get("ANDROID_NDK_ROOT")).absolute().as_posix() + '/build/cmake/android.toolchain.cmake'
        if arch == "armv7":
            cmake_cmd += f' -G Ninja -DCMAKE_TOOLCHAIN_FILE="{toolchain}" -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21'
        else:
            cmake_cmd += f' -G Ninja -DCMAKE_TOOLCHAIN_FILE="{toolchain}" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21'
    elif target == "ios":
        toolchain = Path(project_dir).absolute().as_posix() + '/../cmake/ios.toolchain.cmake'
        cmake_cmd += f' -G Xcode -DCMAKE_TOOLCHAIN_FILE="{toolchain}" -DIOS_DEPLOYMENT_TARGET=11.0 -DPLATFORM=OS64'
    elif target == "macos":
        cmake_cmd += f' -G Xcode -DOSX=1'
    elif target == "emscripten":
        if os.name == 'nt':
            if os.environ['EMSDK'] is not None and os.path.exists(os.environ['EMSDK']):
                os.environ['EMSCRIPTEN_ROOT_PATH'] = os.path.join(os.environ['EMSDK'], 'upstream/emscripten')
            else:
                print('EMSDK is not found, please run "conan install emscripten"')
                os._exit(-1)
        else:
            os.environ['PATH'] = os.environ['PATH'] + ':/usr/local/bin'
            os.environ['EMSCRIPTEN_ROOT_PATH'] = '/usr/local/opt/emscripten/libexec'
            if not os.path.exists(os.environ['EMSCRIPTEN_ROOT_PATH']):
                print('EMSDK is not found, please run "brew install emscripten"')
                os._exit(-1)
        toolchain = Path(os.environ.get("EMSCRIPTEN_ROOT_PATH")).absolute().as_posix() + '/cmake/Modules/Platform/Emscripten.cmake'
        cmake_cmd += f' -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="{toolchain}"'
    else:
        print(f'Configuration not supported: platform = {target}, arch = {arch}')
        exit(1)

    os.system(cmake_cmd)

def _buildCMakeProject(target, arch):
    platform_flags = f"-- /p:CL_MPcount={cpu_count}" if target == "windows" else ""
    os.system(f'cmake --build . --config Release --target install --parallel {cpu_count} {platform_flags}')

def build(target, arch):
    os.chdir(project_dir)
    build_dir = Path(os.path.join('build', target, arch)).as_posix()
    safeRemove(build_dir)
    os.makedirs(build_dir)
    os.chdir(build_dir)
    _generateCMakeProject(target, arch)
    _buildCMakeProject(target, arch)
    os.chdir(project_dir)
    prebuilt_dir = Path(os.path.join('prebuilt', target, arch)).as_posix()
    safeRemove(prebuilt_dir)
    os.makedirs(prebuilt_dir)
    install_dir = os.path.join(build_dir, 'install')
    fnames = os.listdir(install_dir)
    for fname in fnames:
        shutil.move(os.path.join(install_dir, fname), prebuilt_dir)

def main():
    if platform.system() == 'Windows':
        build('windows', 'x86_64')
        build('android', 'armv7')
        build('android', 'armv8')
        build('emscripten', 'wasm')
    elif platform.system() == 'Darwin':
        build('macos', 'x86_64')
        build('ios', 'armv8')

if __name__ == "__main__":
    main()
