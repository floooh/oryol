"
" Local .vimrc for Oryol.
"
" Use a 'local-vim-rc' vim plugin like:
" https://github.com/MarcWeber/vim-addon-local-vimrc
" 
"

" :Fips command maps to oryol python script
command! -nargs=* Fips !python fips <args>

" override :make for compilation
set makeprg=python\ fips\ make

" CtrlP ignore patterns
let g:ctrlp_custom_ignore = '.pyc\|lib\|bin\|sdks\|doc/html'
