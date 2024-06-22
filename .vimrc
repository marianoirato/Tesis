set ignorecase
set scrolloff=8
colorscheme torte
set completeopt=menuone,longest,preview
set fillchars +=vert:\
set number relativenumber
set nu rnu
set showmatch
set lazyredraw
set ttyfast
set hidden
set splitbelow splitright
set wildmenu

function! FoldFunction()
    setlocal foldmethod=marker
    setlocal foldmarker={,}
endfunction

autocmd FileType cpp call FoldFunction()
nnoremap <silent> <C-e> :wincmd w<CR>

nnoremap x zc
nnoremap z zo
nnoremap Z zA

set nobackup
set nowritebackup
set noswapfile
set backspace=indent,eol,start
set clipboard=unnamedplus
set autoindent
set expandtab
set shiftwidth=4
set softtabstop=4
set encoding=utf-8
set ignorecase
set noerrorbells
autocmd FileType python setlocal foldmethod=indent foldlevel=0 foldenable


