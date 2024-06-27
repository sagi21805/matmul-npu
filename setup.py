# setup.py
from pybind11.setup_helpers import Pybind11Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools import setup
import subprocess
import shutil
import os

module_name = "matnpu"

ext_modules = [
    Pybind11Extension(
        f"{module_name}",
        ["src/bindings.cpp"],
        
        include_dirs=["./include", "./", "/usr/local/include/rknpu"],
        library_dirs = ["/usr/local/lib"],
        libraries=['rknnrt'],
        extra_compile_args = ["-fopenmp"]
    ),
]

def generate_init_file(build_lib, module_name):
    # Path to the module directory in the build_lib directory
    module_dir = os.path.join(build_lib, module_name)
    if not os.path.exists(module_dir):
        os.makedirs(module_dir)

    init_file_path = os.path.join(module_dir, '__init__.py')
    with open(init_file_path, 'w') as f:
        f.write(f"from {module_name}.{module_name} import *\n")



class CustomInstallCommand(install):
    """Custom install command to ensure the extension is built before generating stubs."""
    def run(self):
        super().run()
        self.generate_pyi()
        # self.add_docstrings()
        self.cleanup()

    def generate_pyi(self):
        # Run stubgen and place example.pyi in the package directory
        subprocess.run(["stubgen", "-m", f"{module_name}.{module_name}", "-o", self.install_lib, "--include-docstrings"])

    def cleanup(self):
        # Clean up build and egg-info directories
        shutil.rmtree("build", ignore_errors=True)
        shutil.rmtree(module_name + ".egg-info", ignore_errors=True)


class CustomBuildExt(build_ext):
    def run(self):
        # Call the original run method to build the extension
        build_ext.run(self)
        
        # Copy the built .so file to the module directory
        build_lib = self.build_lib
        so_filename = [f for f in os.listdir(build_lib) if f.startswith(module_name) and f.endswith('.so')][0]
        so_source = os.path.join(build_lib, so_filename)
        module_dir = os.path.join(build_lib, module_name)
        
        if not os.path.exists(module_dir):
            os.makedirs(module_dir)
        
        so_dest = os.path.join(module_dir, so_filename)
        shutil.move(so_source, so_dest)
        
        # Generate the __init__.py file
        generate_init_file(build_lib, module_name)



setup(
    name=module_name,
    version="0.0.1",
    author="Your Name",
    description="An example project with C++ and Python",
    ext_modules=ext_modules,
    cmdclass={
        "install": CustomInstallCommand,
        'build_ext': CustomBuildExt},
    zip_safe=False,
)

