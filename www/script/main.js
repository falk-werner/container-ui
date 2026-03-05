import { Api } from "./api.js";
import { login } from "./login.js";
import { format_mem, set_text} from "./util.js";

async function fetch_json(url) {
    const response = await fetch(url);
    return await response.json();
}


async function engine_info(api) {
    const data = await api.version();
    set_text("#engine_name", data.Platform.Name);
    set_text("#engine_version", data.Version);
}

async function system_info(api) {
    const data = await api.info();
    set_text("#system_os", data.OperatingSystem);
    set_text("#system_arch", data.Architecture);
    set_text("#system_ncpu", data.NCPU);
    set_text("#system_memory", format_mem(data.MemTotal));
}

async function disk_usage(api) {
    const data = await api.disk_usage();

    set_text("#df_container_count", data.ContainerUsage.TotalCount);
    set_text("#df_container_total", format_mem(data.ContainerUsage.TotalSize));
    set_text("#df_container_reclaimable", format_mem(data.ContainerUsage.Reclaimable));

    set_text("#df_image_count", data.ImageUsage.TotalCount);
    set_text("#df_image_total", format_mem(data.ImageUsage.TotalSize));
    set_text("#df_image_reclaimable", format_mem(data.ImageUsage.Reclaimable));

    set_text("#df_volume_count", data.VolumeUsage.TotalCount);
    set_text("#df_volume_total", format_mem(data.VolumeUsage.TotalSize));
    set_text("#df_volume_reclaimable", format_mem(data.VolumeUsage.Reclaimable));

    set_text("#df_bcache_count", data.BuildCacheUsage.TotalCount);
    set_text("#df_bcache_total", format_mem(data.BuildCacheUsage.TotalSize));
    set_text("#df_bcache_reclaimable", format_mem(data.BuildCacheUsage.Reclaimable));
}


async function containers(api) {
    const data = await api.containers();
    const element = document.querySelector("#containers_list");
    element.innerHTML = "";

    for(const container of data) {
        const tr = document.createElement("tr");
        tr.addEventListener("click", () => {
            activate_container(api, container.Id);
        });
        element.appendChild(tr);

        const name = document.createElement("td");
        name.textContent = container.Names[0].substring(1);
        tr.appendChild(name);

        const state = document.createElement("td");
        state.textContent = container.State;
        tr.appendChild(state);

        const image = document.createElement("td");
        image.textContent = container.Image;
        tr.appendChild(image);

        const rootfs = document.createElement("td");
        rootfs.textContent = format_mem(container.SizeRootFs);
        tr.appendChild(rootfs);
    }    
}

async function images(api) {
    const data = await api.images();
    const element = document.querySelector("#images_list");

    for(const image of data) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const tags_id = document.createElement("td");
        tr.appendChild(tags_id);

        const tags = document.createElement("div");
        tags.textContent = image.RepoTags.join(" ");
        tags_id.appendChild(tags);

        const id = document.createElement("div");
        id.textContent = image.Id;
        tags_id.appendChild(id);

        const created = document.createElement("td");
        created.textContent = new Date(image.Created * 1000).toISOString();
        tr.appendChild(created);

        const size = document.createElement("td");
        size.textContent = format_mem(image.Size);
        tr.appendChild(size);

    }
}

async function volumes(api) {
    const data = await api.volumes();
    const df = await api.disk_usage();

    const element = document.querySelector("#volumes_list");
    element.innerHTML = "";

    for(const volume of data.Volumes) {
        const tr = document.createElement("tr");
        tr.addEventListener("click", () => { activate_volume(api, volume.Name); });
        element.appendChild(tr);

        const name = document.createElement("td");
        name.textContent = volume.Name;
        tr.appendChild(name);

        const created = document.createElement("td");
        created.textContent = volume.CreatedAt;
        tr.appendChild(created);

        const info = get_volume_info(df, volume.Name);

        const ref_count = document.createElement("td");
        ref_count.textContent = (info) ? info.UsageData.RefCount : "unknown";
        tr.appendChild(ref_count);

        const volume_size = document.createElement("td");
        volume_size.textContent = (info) ? format_mem(info.UsageData.Size) : "unknown";
        tr.appendChild(volume_size);
    }
}

function activate_page(name) {
    const pages = document.querySelectorAll(".page");
    for(const page of pages) {
        page.classList.add("hidden");
    }

    const page = document.querySelector(`#${name}`);
    page.classList.remove("hidden");
}

function activate_entry(name) {
    const menuentries = document.querySelectorAll(".mainmenu li");
    for(const entry of menuentries) {
        entry.classList.remove("active");
    }
    const entry = document.querySelector(`#menuentry_${name}`);
    entry.classList.add("active");

    activate_page(name);
}

function init_home(api)
{
    document.querySelector("#menuentry_home").addEventListener("click", () => {
        activate_home(api);
    });

    document.querySelector("#home_containers").addEventListener("click", () => {
        activate_containers(api);
    });

    document.querySelector("#home_images").addEventListener("click", () => {
        activate_images(api);
    });

    document.querySelector("#home_volumes").addEventListener("click", () => {
        activate_volumes(api);
    });

}

async function activate_home(api)
{
    const version = await api.version();
    set_text("#home_engine_version", `${version.Platform.Name} / ${version.Version}`);

    const info = await api.info();
    set_text("#home_name", info.Name);
    set_text("#home_arch", info.Architecture);
    set_text("#home_ncpu", info.NCPU);
    set_text("#home_memory", format_mem(info.MemTotal));

    activate_entry("home");
    const df = await api.disk_usage();
    let running = 0;
    for(const container of df.Containers) {
        if (container.State === "running") {
            running++;
        }
    }

    document.querySelector("#home_containers_running").textContent = running;
    document.querySelector("#home_containers_total").textContent = df.ContainerUsage.TotalCount;
    document.querySelector("#home_images_active").textContent = df.ImageUsage.ActiveCount;
    document.querySelector("#home_images_total").textContent = df.ImageUsage.TotalCount;
    document.querySelector("#home_volumes_active").textContent = df.VolumeUsage.ActiveCount;
    document.querySelector("#home_volumes_total").textContent = df.VolumeUsage.TotalCount;
}

function init_containers(api)
{
    document.querySelector("#menuentry_containers").addEventListener("click", () => {
        activate_containers(api);
    });

    document.querySelector("#container_update").addEventListener("click", () => {
        const id = document.querySelector("#container_name").textContent;
        activate_container(api, id);
    });

    document.querySelector("#container_start").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_start(id);
        activate_container(api, id);
    });

    document.querySelector("#container_stop").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_stop(id);
        activate_container(api, id);
    });

    document.querySelector("#container_pause").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_pause(id);
        activate_container(api, id);
    });


    document.querySelector("#container_unpause").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_unpause(id);
        activate_container(api, id);
    });


    document.querySelector("#container_restart").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_restart(id);
        activate_container(api, id);
    });


    document.querySelector("#container_kill").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_kill(id);
        activate_container(api, id);
    });

    document.querySelector("#container_remove").addEventListener("click", async () => {
        const id = document.querySelector("#container_name").textContent;
        await api.container_remove(id);
        activate_containers(api);
    });
}

async function activate_containers(api)
{
    activate_entry("containers");
    containers(api);
}

async function activate_container(api, id) {
    activate_page("container");
    const data = await api.container_inspect(id);
    const stats = await api.container_stats(id);
    const image = await api.image_inspect(data.Image.substring(7));
    
    set_text("#container_name", data.Name.substring(1));
    set_text("#container_state", data.State.Status);
    set_text("#container_image", image.RepoTags[0]);

}

async function activate_images(api)
{
    activate_entry("images");
    images(api);
}

function init_volumes(api)
{
    document.querySelector("#menuentry_volumes").addEventListener("click", () => {
        activate_volumes(api);
    });

    document.querySelector("#volumes_new").addEventListener('click', async () => {
        try {
            const name = document.querySelector("#volumes_new_name");
            await api.volume_create(name.value);
            activate_volumes(api);
        }
        catch (ex) {
            console.log(ex);
        }
    });

    document.querySelector("#volumes_prune").addEventListener('click', async () => {
        try {
            await api.volumes_prune();
            activate_volumes(api);
        }
        catch (ex) {
            console.log(ex);
        }
    });

    document.querySelector("#volume_remove").addEventListener('click', async () => {
        try {
            const name = document.querySelector("#volume_name").textContent;
            await api.volume_remove(name);
            activate_volumes(api);
        }
        catch (ex) {
            console.log(ex);
        }
    });
}

async function activate_volumes(api)
{
    activate_entry("volumes");
    volumes(api);
}

function get_volume_info(df, name) {
    for(const volume of df.Volumes) {
        if (volume.Name == name) {
            return volume;
        }
    }

    return null;
    // throw new Error("volume not found");
}

async function activate_volume(api, name) {
    activate_page("volume");
    const volume = await api.volume_inspect(name);
    const df = await api.disk_usage();
    const info = get_volume_info(df, name);

    set_text("#volume_name", volume.Name);
    set_text("#volume_created_at", volume.CreatedAt);
    set_text("#volume_ref_count", (info) ? info.UsageData.RefCount : "unknown");
    set_text("#volume_size", (info) ? format_mem(info.UsageData.Size) : "unknown");
    set_text("#volume_driver", volume.Driver);
    set_text("#volume_scope", volume.Scope);
    set_text("#volume_mountpoint", volume.Mountpoint);

    const labels_element = document.querySelector("#volume_labels");
    labels_element.innerHTML = "";
    if ((volume.Labels) && (Object.keys(volume.Labels).length > 0)) {

        for(const [name, value] of Object.entries(volume.Labels)) {
            const tr = document.createElement("tr");
            labels_element.appendChild(tr);

            const td_name = document.createElement("td");
            td_name.textContent = name;
            tr.appendChild(td_name);

            const td_value = document.createElement("td");
            td_value.textContent = value;
            tr.appendChild(td_value);
        }
    }
    else {
        const tr = document.createElement("tr");
        labels_element.appendChild(tr);

        const td = document.createElement("td");
        td.setAttribute("colspan", "2");
        td.textContent = "No Labels set";
        tr.appendChild(td);
    }

    const opts_element = document.querySelector("#volume_options");
    opts_element.innerHTML = "";
    if ((volume.Options) && (Object.keys(volume.Options).length > 0)) {

        for(const [name, value] of Object.entries(volume.Options)) {
            const tr = document.createElement("tr");
            opts_element.appendChild(tr);

            const td_name = document.createElement("td");
            td_name.textContent = name;
            tr.appendChild(td_name);

            const td_value = document.createElement("td");
            td_value.textContent = value;
            tr.appendChild(td_value);
        }
    }
    else {
        const tr = document.createElement("tr");
        opts_element.appendChild(tr);

        const td = document.createElement("td");
        td.setAttribute("colspan", "2");
        td.textContent = "No Options present";
        tr.appendChild(td);
    }
}


async function activate_system(api)
{
    activate_entry("system");
    engine_info(api);
    system_info(api);
    disk_usage(api);
}

async function startup() {
    const access_token = await login();


    const splash = document.querySelector("#splash");
    splash.classList.add("hidden");

    if (access_token) {
        const main = document.querySelector("#main");
        main.classList.remove("hidden");

        const api = new Api(access_token);
        init_home(api);
        init_containers(api);

        document.querySelector("#menuentry_images").addEventListener("click", () => {
            activate_images(api);
        });

        init_volumes(api);

        document.querySelector("#menuentry_system").addEventListener("click", () => {
            activate_system(api);
        });

        activate_home(api);
    }
    else {
        const login_failed = document.querySelector("#login_failed");
        login_failed.classList.remove("hidden");
    }
}


if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", startup());
} else {
  startup();
}
