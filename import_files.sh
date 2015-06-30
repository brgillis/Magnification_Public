export MY_RPATH='/disk2/brg/git'
export MY_LIBPATH=$MY_RPATH'/brg_library'
export MY_CATPATH=$MY_RPATH'/CFHTLenS_cat'

cp -rf $MY_LIBPATH/IceBRG_main/IceBRG_main \
src/lib/IceBRG_main
cp -rf $MY_LIBPATH/IceBRG_physics/IceBRG_physics \
src/lib/IceBRG_physics
cp -rf $MY_LIBPATH/IceBRG_lensing/IceBRG_lensing \
src/lib/IceBRG_lensing

cp -rf $MY_CATPATH/CFHTLenS_catalogue_filtering/src \
src/exec/CFHTLenS_catalogue_filtering
cp -rf $MY_CATPATH/CFHTLenS_corr_func/src \
src/exec/CFHTLenS_corr_func
cp -rf $MY_CATPATH/CFHTLenS_field_stats/src \
src/exec/CFHTLenS_field_stats
cp -rf $MY_CATPATH/CFHTLenS_gg_lensing/src \
src/exec/CFHTLenS_gg_lensing
cp -rf $MY_CATPATH/CFHTLenS_make_mock_lens_cats/src \
src/exec/CFHTLenS_make_mock_cats
cp -rf $MY_CATPATH/CFHTLenS_mask_testing/src \
src/exec/CFHTLenS_get_unmasked_fractions
cp -rf $MY_CATPATH/CFHTLenS_source_count_fitting/src \
src/exec/CFHTLenS_source_count_fitting
cp -rf $MY_CATPATH/CFHTLenS_source_counting/src \
src/exec/CFHTLenS_source_counting
cp -rf $MY_CATPATH/Lensing_model_fitter/src \
src/exec/CFHTLenS_fit_lensing_models
