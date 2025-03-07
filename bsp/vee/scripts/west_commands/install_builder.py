'''install_builder.py

Install cmakes file as symlinks.'''

from textwrap import dedent            # just for nicer code indentation

from west.commands import WestCommand  # your extension must subclass this
from west import log                   # use this for user output
import os
import subprocess

class InstallBuilder(WestCommand):

    def __init__(self):
        super().__init__(
            'install-builder',  # gets stored as self.name
            'Install cmake files as symlinks',  # self.help
            # self.description:
            dedent('''
            Enable optional repo, fetch the code
            and install cmake files as symlinks
            '''))

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        if os.path.isfile("CMakeLists.txt"):
            subprocess.run(['make', '-B', '.preset.gen'], check=True, text=True)
        else:
            log.inf("Add optional repo")
            subprocess.run(['west', 'config', 'manifest.group-filter', '+optional'], check=True, text=True)
            log.inf("update west tree")
            subprocess.run(['west', 'update'], check=True, text=True)
            log.inf("link files")
            os.symlink(".builder/Makefile", "Makefile");
            os.symlink(".builder/CMakeLists.txt", "CMakeLists.txt");
            os.symlink(".builder/CMakePresets.json", "CMakePresets.json");
            subprocess.run(['cp', '.builder/Makefile.inc', "."], check=True, text=True)
            subprocess.run(['make', '.preset.gen'], check=True, text=True)

