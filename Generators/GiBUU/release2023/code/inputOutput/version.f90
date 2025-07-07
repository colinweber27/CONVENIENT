module version
 contains  
 subroutine PrintVersion 
 implicit none 
 write(*,2) "Version    : Release 2023, patch 3 (February 21, 2024)" 
 write(*,2) "Build Date : Mon Jul 7 11:16:06 CDT 2025 " 
 write(*,2) "OS         : Linux 5.14.0-570.21.1.el9_6.x86_64 x86_64" 
 write(*,2) "Compiler   : gcc version 9.3.0 (GCC) " 
 write(*,2) "Flags      : -O3 -ffree-line-length-none -Wno-align-commons -g -fbacktrace -std=legacy " 
 write(*,2) "PATH       : /exp/nova/app/users/colweber/Convenient/Generators/GiBUU/release2023 " 
 2 FORMAT(" ",A)
 end subroutine PrintVersion 
end module version 
