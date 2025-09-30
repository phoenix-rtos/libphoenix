FROM python:3.12-slim-bookworm

# Install needed debian packages
RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install \
    sudo \
    clang \
    llvm \
    libclang-dev \
 && apt-get clean && rm -rf /var/lib/apt/lists/*

# Add a default developer user
RUN useradd -m -G sudo developer \
    && echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Install dependencies
ADD pyproject.toml poetry.lock /
RUN pip install poetry && poetry config virtualenvs.create false && poetry install --no-root
# Open Links to GitLab in a new tab
RUN sed -i 's/fa-gitlab">/fa-gitlab" target="_blank">/' /usr/local/lib/python3.*/site-packages/sphinx_rtd_theme/breadcrumbs.html
