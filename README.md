# Text Editor

Elegant text editor for LingmoOS. 

## Dependencies

### Debian/Ubuntu

```
sudo apt install equivs curl git devscripts lintian build-essential automake autotools-dev --no-install-recommends
```

## Build

```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
make
```

## Install

```shell
sudo make install
```

## License

This project has been licensed by GPLv3.

## Notas locais (lingmo-notes)

O build também gera `lingmo-notes`, com atalho **Notas** no menu de aplicativos.
Oferece páginas, busca no título e conteúdo, edição/visualização Markdown e
salvamento automático após 600 ms. `Ctrl+N` cria uma página, `Ctrl+F` busca e
`Ctrl+S` salva imediatamente. A troca de página e o fechamento salvam antes de
continuar; falhas de gravação mantêm o rascunho aberto.

Os arquivos ficam em `$XDG_DATA_HOME/lingmoos/lingmo-notes` (normalmente
`~/.local/share/lingmoos/lingmo-notes`). Cada página é um `.md` identificado por
UUID, com título na primeira linha. A pasta `Trash` preserva páginas excluídas;
para restaurar, feche o app e mova o arquivo de volta para a pasta de notas.
Faça backup dessa pasta. Este MVP é local, sem sincronização ou colaboração.

```sh
cmake -S notes/tests -B /tmp/lingmo-notes-tests
cmake --build /tmp/lingmo-notes-tests
ctest --test-dir /tmp/lingmo-notes-tests --output-on-failure
```
