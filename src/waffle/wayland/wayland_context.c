// Copyright 2012 Intel Corporation
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define WL_EGL_PLATFORM 1

#include <stdlib.h>

#include <waffle/core/wcore_config.h>
#include <waffle/core/wcore_error.h>

#include "wayland_config.h"
#include "wayland_context.h"
#include "wayland_display.h"
#include "wayland_priv_egl.h"

static const struct wcore_context_vtbl wayland_context_wcore_vtbl;

static bool
wayland_context_destroy(struct wcore_context *wc_self)
{
    struct wayland_context *self = wayland_context(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->egl)
        ok &= egl_destroy_context(wayland_display(wc_self->display)->egl,
                                  self->egl);

    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_context*
wayland_context_create(struct wcore_platform *wc_plat,
                    struct wcore_config *wc_config,
                    struct wcore_context *wc_share_ctx)
{
    struct wayland_context *self;
    struct wayland_config *config = wayland_config(wc_config);
    struct wayland_context *share_ctx = wayland_context(wc_share_ctx);
    struct wayland_display *dpy = wayland_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_context_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->egl = egl_create_context(dpy->egl,
                                   config->egl,
                                   share_ctx
                                      ? share_ctx->egl
                                      : NULL,
                                   config->waffle_context_api);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &wayland_context_wcore_vtbl;
    return &self->wcore;

error:
    wayland_context_destroy(&self->wcore);
    return NULL;
}

static union waffle_native_context*
wayland_context_get_native(struct wcore_context *wc_self)
{
    struct wayland_context *self = wayland_context(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);
    struct waffle_wayland_context *n_ctx;

    n_ctx = wcore_malloc(sizeof(*n_ctx));
    if (n_ctx == NULL)
        return NULL;

    wayland_display_fill_native(dpy, &n_ctx->display);
    n_ctx->egl_context = self->egl;

    return (union waffle_native_context*) n_ctx;
}

static const struct wcore_context_vtbl wayland_context_wcore_vtbl = {
    .destroy = wayland_context_destroy,
    .get_native = wayland_context_get_native,
};
