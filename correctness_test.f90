program correctness_test
  use iso_fortran_env, only: int64, real64
  use m_random

  implicit none

  integer, parameter        :: dp        = real64
  integer(int64), parameter :: n_samples = 1000*1000*1000
  integer(int64)            :: seed(2), random_int64, n
  real(dp)                  :: time_start, time_end
  type(RNG_t)               :: rng

  seed = [1234_int64, -89234_int64]
  call rng%set_seed(seed, do_jump=.false.)

  print *, "Initial state: ", seed

  call cpu_time(time_start)
  do n = 1, n_samples
     random_int64 = rng%int_64()
  end do
  call cpu_time(time_end)

  print *, "Random value ", n_samples, " is: ", random_int64
  print *, "nanoseconds per number: ", 1.0e9_dp * (time_end - time_start) / n_samples

  if (random_int64 == -6590794730266477748_int64) then
     print *, "PASSED"
  else
     error stop "FAILED: expected -6590794730266477748"
  end if

  call rng%jump()
  random_int64 = rng%int_64()

  print *, ""
  print *, "Result after calling jump()"
  print *, "Random value is: ", random_int64

  if (random_int64 == -7370777279832017598_int64) then
     print *, "PASSED"
  else
     error stop "FAILED: expected -7370777279832017598"
  end if

end program correctness_test
