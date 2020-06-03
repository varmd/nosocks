#Copyright (C) 2020 varmd

pkgname=nosocks
pkgver=1
pkgrel=6
pkgdesc='Control access to the internet for non-static apps on Linux.'
url=''
license=('GPL')
arch=('x86_64')
depends=(glibc gcc)
install=nosocks.install

backup=("etc/${pkgname}/${pkgname}.conf"

)

build() {
	cd "${srcdir}/${pkgname}"
	gcc -fPIC -march=x86-64 -mtune=generic -O2 -pipe -fno-plt -w  -I. *.c -nostdlib -shared -o libnosocks.so -ldl -lc
}

package() {
  install=nosocks.install
	cd "${srcdir}/${pkgname}"
	mkdir -p "${pkgdir}/usr/lib/"
	mkdir -p "${pkgdir}/etc/nosocks/"
	install libnosocks.so "${pkgdir}/usr/lib/libnosocks.so"
	install default.conf "${pkgdir}/etc/nosocks/nosocks.conf"
  rm -rf libnosocks.so
  rm -rf *.o
	
}
