set nocompatible
source $VIMRUNTIME/vimrc_example.vim
source $VIMRUNTIME/mswin.vim
behave mswin

set diffexpr=MyDiff()
function MyDiff()
let opt = '-a --binary '
if &diffopt =~ 'icase' | let opt = opt . '-i ' | endif
if &diffopt =~ 'iwhite' | let opt = opt . '-b ' | endif
let arg1 = v:fname_in
if arg1 =~ ' ' | let arg1 = '"' . arg1 . '"' | endif
let arg2 = v:fname_new
if arg2 =~ ' ' | let arg2 = '"' . arg2 . '"' | endif
let arg3 = v:fname_out
if arg3 =~ ' ' | let arg3 = '"' . arg3 . '"' | endif
let eq = ''
if $VIMRUNTIME =~ ' '
	if &sh =~ '\<cmd'
		let cmd = '""' . $VIMRUNTIME . '\diff"'
		let eq = '"'
	else
		let cmd = substitute($VIMRUNTIME, ' ', '" ', '') . '\diff"'
	endif
else
		let cmd = $VIMRUNTIME . '\diff'
endif
silent execute '!' . cmd . ' ' . opt . arg1 . ' ' . arg2 . ' > ' . arg3 . eq
endfunction

"=========================================================
set nu
set ts=4
set sw=4
set nobackup
"set background=light
set guifont=Courier\New\ 11
set noerrorbells
set novisualbell
set cindent
set autoindent
set smartindent
syntax on
filetype plugin on
filetype indent on
colorscheme dante

set fileencodings=utf-8,gbk,gb2312

nmap <F2> i<CR><Esc>k<Esc>o
"nmap <F3> :w!<CR>
nmap <F4> <C-w>w
nmap <F5> :copen<CR>
nmap <F6> :cclose<CR>
nmap <F7> :make<CR>
nmap <F8> :NERDTree<CR>
nmap <F10> :Tlist<CR>

"nnoremap <silent> <F3> :Grep<CR>
