TOP  = ..

PREFIX = /usr/local

INCLUDE_DIR   = $(TOP)/include
LIBDIR        = $(TOP)/lib
GENCODECS_DIR = $(TOP)/gencodecs
CORE_DIR      = $(TOP)/core

CORE_OBJS     = $(CORE_DIR)/cog-utils.o      \
                $(CORE_DIR)/io_poller.o      \
                $(CORE_DIR)/user-agent.o     \
                $(CORE_DIR)/websockets.o     \
                $(CORE_DIR)/curl-websocket.o \
                $(CORE_DIR)/jsmn-find.o      \
                $(CORE_DIR)/json-build.o     \
                $(CORE_DIR)/log.o            \
                $(CORE_DIR)/logconf.o        \
                $(CORE_DIR)/priority_queue.o \
                $(CORE_DIR)/anomap.o         \
                $(CORE_DIR)/sha1.o           \
                $(CORE_DIR)/threadpool.o     \
                $(CORE_DIR)/queriec.o
GENCODECS_OBJ = $(GENCODECS_DIR)/winecord_codecs.o
VOICE_OBJS    = winecord-voice.o

OBJS += winecord-once.o             \
        winecord-refcount.o         \
        winecord-rest.o             \
        winecord-rest_request.o     \
        winecord-rest_ratelimit.o   \
        winecord-client.o           \
        winecord-events.o           \
        winecord-cache.o            \
        winecord-loop.o             \
        winecord-gateway.o          \
        winecord-gateway_dispatch.o \
        winecord-messagecommands.o  \
        winecord-timer.o            \
        winecord-misc.o             \
        winecord-worker.o           \
        application_command.o      \
        auto_moderation.o          \
        interaction.o              \
        audit_log.o                \
        channel.o                  \
        emoji.o                    \
        gateway.o                  \
        guild.o                    \
        guild_scheduled_event.o    \
        guild_template.o           \
        invite.o                   \
        oauth2.o                   \
        user.o                     \
        voice.o                    \
        webhook.o

WFLAGS  = -Wall -Wextra -Wshadow -Wdouble-promotion -Wconversion -Wpedantic
CFLAGS += -std=c99 -pthread -D_XOPEN_SOURCE=600 -DLOG_USE_COLOR \
          -I$(INCLUDE_DIR) -I$(CORE_DIR) -I$(GENCODECS_DIR) -I$(PREFIX)/include

ARLIB = $(LIBDIR)/libwinecord.a
SOLIB = $(LIBDIR)/libwinecord.so
DYLIB = $(LIBDIR)/libwinecord.dylib

ARFLAGS = -cqsv
SOFLAGS = -fPIC
DYFLAGS = -fPIC 

.c.o:
	$(CC) -c $(CFLAGS) $(WFLAGS) -o $@ $<

all: static

static: $(ARLIB)

shared:
	@ CFLAGS="$(SOFLAGS)" $(MAKE) $(SOLIB)
shared_osx:
	@ CFLAGS="$(DYFLAGS)" $(MAKE) $(DYLIB)

$(ARLIB): deps
	$(AR) $(ARFLAGS) $@ $(OBJS) $(GENCODECS_OBJ) $(CORE_OBJS)
$(SOLIB): deps
	$(CC) -shared -lcurl -o $@ $(OBJS) $(GENCODECS_OBJ) $(CORE_OBJS)
$(DYLIB): deps
	$(CC) -dynamiclib $(DYFLAGS) -o $@ $(OBJS) $(GENCODECS_OBJ) $(CORE_OBJS)

deps:
	@ $(MAKE) -C $(CORE_DIR)
	@ $(MAKE) -C $(GENCODECS_DIR)
	@ $(MAKE) $(OBJS)

echo:
	@ echo -e 'CC: $(CC)\n'
	@ echo -e 'PREFIX: $(PREFIX)\n'
	@ echo -e 'CFLAGS: $(CFLAGS)\n'
	@ echo -e 'GENCODECS_OBJ: $(GENCODECS_OBJ)\n'
	@ echo -e 'CORE_OBJS: $(CORE_OBJS)\n'
	@ echo -e 'VOICE_OBJS: $(VOICE_OBJS)\n'
	@ echo -e 'OBJS: $(OBJS)\n'

voice:
	@ CFLAGS="-DWINEBERRY_VOICE" OBJS="$(VOICE_OBJS)" $(MAKE)

clean: 
	@ rm -rf $(LIBDIR)/*
	@ rm -f $(OBJS) $(VOICE_OBJS)
	@ $(MAKE) -C $(CORE_DIR) clean
purge: clean
	@ $(MAKE) -C $(GENCODECS_DIR) clean

.PHONY: test examples install echo clean purge docs deps static shared shared_osx
