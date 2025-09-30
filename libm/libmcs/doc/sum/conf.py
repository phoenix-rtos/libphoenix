# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a
# full list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup -------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another
# directory, add these directories to sys.path here. If the directory is
# relative to the documentation root, use os.path.abspath to make it
# absolute, like shown here.
#
import os
import sys
from shutil import copyfile
import sphinx_rtd_theme  # pylint: disable=unused-import
from sphinx.errors import ConfigError

sys.path.insert(0, os.path.abspath(".."))

# -- Project information ----------------------------------------------

project = "LibmCS - SUM"
copyright = "2025, GTD GmbH"
author = "GTD GmbH"


# -- General configuration --------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "matplotlib.sphinxext.plot_directive",
    "sphinx.ext.autodoc",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.mathjax",
    "sphinx-mathjax-offline",
    "sphinx_rtd_theme",
    "hawkmoth",
]

# Figures are referenced by number
numfig = True

# Configuration for hawkmoth
hawkmoth_clang = ["-Ilibm/include", "-Isw-quality/dummy_includes"]
hawkmoth_root = os.path.abspath("../../libm")

# Add any paths that contain templates here, relative to this directory.
templates_path = ["../_templates"]

# -- Options for HTML output ------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation
# for a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"

html_theme_options = {
    "collapse_navigation": False,
    "logo_only": True,
}

html_logo = "../logo/libmcs-logo.png"
html_favicon = "../logo/libmcs-favicon.ico"

# Add any paths that contain custom static files (such as style sheets)
# here, relative to this directory. They are copied after the builtin
# static files, so a file named "default.css" will overwrite the builtin
# "default.css".
html_static_path = ["../_static"]

html_css_files = ["css/custom.css"]

html_context = {
    "display_gitlab": True,  # Integrate Gitlab
    "gitlab_host": "gitlab.com",
    "gitlab_user": "gtd-gmbh",  # Organization or User
    "gitlab_repo": "libmcs",  # Repo name
    "gitlab_version": "development",  # Version
    "conf_py_path": "/doc/sum/",  # Path in the checkout to the docs root
}
