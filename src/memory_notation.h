/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)BSD-3-Clause
 *
 * SPDX short identifier: BSD-3-Clause
 *
 * Copyright 2024 P.L. Lucas <selairi@gmail.com>
 * 
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided 
 * that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the following
 * disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the
 * above copyright notice, this list of conditions and the
 * following disclaimer in the documentation and/or other 
 * materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names
 * of its contributors may be used to endorse or promote
 * products derived from this software without specific prior
 * written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef __MEMORY_NOTATION_H__
#define __MEMORY_NOTATION_H__

#define memory_guarded
#define memory_owner
#define memory_take_possession
#define memory_keep_alive
#define memory_release_after_of(mem)
#define memory_owner_of(mem)
#define memory_ref_count
#define memory_ptr_inout
#define memory_ptr_out

#define m_g
#define m_o
#define m_t
#define m_o_(mem)
#define m_rc
#define m_io
#define m_out

#define memory_cleanup_(f) __attribute__((cleanup(f)))

// Example:
//
// struct Example {
// 	memory_guarded char *name; // This pointer must not be freed by this object
// 	memory_owner   char *id;     // This object must be freed by this object
// };
// 
// memory_take_possession struct Example *example_new(
//     memory_guarded        char *name, 
//     memory_take_possession char *id
// ) 
// {
// 	struct Example *ex = (struct Example *)malloc(sizeof(struct Example));
// 	ex->name = name; ex->id = id;
// 	return ex;
// }
// 
// void example_delete(memory_owner struct Example *ex) {
// 	free(ex->id);
// 	free(ex);
// }
// 
// memory_guarded char *example_id(memory_guarded struct Example *ex) {
// 	return ex->id;
// }
// 
// int main() {
//   const char *name = "Hello";
//   struct Example *ex = example_new((char*)name, strdup("World"));
//   printf("%s\n", example_id(ex));
//   example_delete(ex);
// }
#endif
