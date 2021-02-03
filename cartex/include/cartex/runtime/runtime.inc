runtime::runtime(
    int num_reps, int ext, int halo, int num_fields, bool check_res, decomposition& decomp_)
: m_decomposition(decomp_)
, m_rank(m_decomposition.rank())
, m_size(m_decomposition.size())
, m_num_reps{num_reps}
, m_num_threads(m_decomposition.threads_per_rank())
, m_mt(m_num_threads > 1)
, m_num_fields{num_fields}
, m_check_res(check_res)
, m_ext{ext, ext, ext}
, m_g_first{0, 0, 0}
, m_g_last{m_decomposition.last_coord()[0] * m_ext[0] + m_ext[0] - 1,
      m_decomposition.last_coord()[1] * m_ext[1] + m_ext[1] - 1,
      m_decomposition.last_coord()[2] * m_ext[2] + m_ext[2] - 1}
, m_offset{halo, halo, halo}
, m_halos{halo, halo, halo, halo, halo, halo}
, m_ext_buffer{m_ext[0] + m_halos[0] + m_halos[1], m_ext[1] + m_halos[2] + m_halos[3],
      m_ext[2] + m_halos[4] + m_halos[5]}
, m_max_memory{m_ext_buffer[0] * m_ext_buffer[1] * m_ext_buffer[2]}
, m_raw_fields(m_num_threads)
, m_impl{std::make_unique<impl>(*this)}
{
}

runtime::~runtime() = default;

std::string
runtime::info() const
{
    return m_impl->info();
}

void
runtime::init(int j)
{
    m_impl->init(j);
}

void
runtime::step(int j)
{
    m_impl->step(j);
}

