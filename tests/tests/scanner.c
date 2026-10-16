#include <stdio.h>
#include "../all.h"
void main() {
    Z_Scanner s=z_scanner_new(z_sv("  abc,xy\nend"));
    z_scanner_skip_spaces(&s); char first=z_scanner_peek(&s); bool checked=z_scanner_check(&s,'a'); bool matched=z_scanner_match(&s,'a');
    printf("peek=%c check=%d match=%d\n",first,checked,matched);
    z_scanner_advance_until(&s,','); z_sv_println(z_scanner_capture(&s));
    z_scanner_reset_mark(&s); printf("comma=%d ",z_scanner_match(&s,','));
    printf("match-string=%d ",z_scanner_match_string(&s,z_sv("xy"))); z_scanner_advance(&s,1); printf("prev=%c line=%zu col=%zu\n",z_scanner_previous(&s),s.line,s.column);
    z_scanner_advance_until_string(&s,z_sv("end")); printf("check-string=%d ",z_scanner_check_string(&s,z_sv("end"))); z_sv_println(z_scanner_capture(&s));
    z_scanner_reset_mark(&s); z_scanner_advance(&s,3); printf("end=%d\n",z_scanner_is_at_end(&s));
    Z_Scanner t=z_scanner_new(z_sv("!?!x")); z_scanner_skip_cset(&t,z_sv("!?")); printf("skip=%c\n",z_scanner_peek(&t));
}
