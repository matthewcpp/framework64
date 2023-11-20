import subprocess

subprocess.call('doxygen', shell=True)

html_theme = "alabaster"
project = 'framework64'
copyright = '2023, Matthew LaRocca'
author = 'Matthew LaRocca'


master_doc = 'index'

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
html_extra_path = ['_build/doxygen']